#include <stdlib.h>
#include <stdio.h>

#include <dbDefs.h>
#include <ellLib.h>
#include <devSup.h>
#include <recSup.h>
#include <recGbl.h>
#include <callback.h>
#include <initHooks.h>
#include <dbAccess.h>
#include <epicsExport.h>
#include <cantProceed.h>
#include <devLib.h>
#include <iocsh.h>

#include <motorRecord.h>
#include <motor.h>

/* Simulated hardware state */
struct hardware {
	epicsInt32 pos; /* signed */
	epicsInt32 lim_h_val;
	int lim_h:1;
	epicsInt32 lim_l_val;
	int lim_l:1;
};

enum {max_targs=2};

typedef int (*trans_proc_t)();

/* List entry to hold transactions */
struct trans {
	ELLNODE node; /* Must be first */
	size_t nargs;
	double args[max_targs];

	trans_proc_t trans_proc;
};

/* Device support private */
struct devsim {
	ELLNODE node;

	int id;

	double rate; /* poll rate */

	struct hardware hw;

	int updateReady;
	CALLBACK updatecb;

	ELLLIST transaction; /* list of struct trans */
};

static
ELLLIST devices = {{NULL,NULL},0}; /* list of struct devsim */

static
struct devsim *getDev(int id)
{
	ELLNODE *node;
	struct devsim *cur;

	for(node=ellFirst(&devices); node; node=ellNext(node))
	{
		cur=(struct devsim*)node;
		if(cur->id==id)
			return cur;
	}
	return NULL;
}

static
void timercb(CALLBACK* cb)
{
	motorRecord *pmr=NULL;
	struct rset* rset=NULL;
	struct devsim *priv=NULL;

	callbackGetUser(pmr,cb);
	priv=pmr->dpvt;
	rset=(struct rset*)pmr->rset;

	callbackRequestDelayed(&priv->updatecb, 1.0/priv->rate);

	priv->updateReady=1;

	dbScanLock((dbCommon*)pmr);
	(*rset->process)(pmr);
	dbScanUnlock((dbCommon*)pmr);
}

static
long init_record(motorRecord *pmr)
{
	struct motor_dset *dset=(struct motor_dset*)(pmr->dset);
	msta_field stat;
	double val;
	long ret=0;
	struct devsim *priv=getDev(pmr->out.value.vmeio.card);

	if(!priv){
		printf("Invalid id %d\n",pmr->out.value.vmeio.card);
		ret=S_dev_noDevice;
		goto error;
	}
	pmr->dpvt=priv;

	callbackSetUser(pmr, &priv->updatecb);

	stat.Bits.RA_DONE=1;

	pmr->msta=stat.All;

	if( pmr->mres !=0.0 ){
		/* Restore position counter.  Needed for autosave */
		val = pmr->dval / pmr->mres;
		(*dset->start_trans)(pmr);
		(*dset->build_trans)(LOAD_POS, &val, pmr);
		(*dset->end_trans)(pmr);
	}

	return 0;
error:
	pmr->pact=TRUE;
	return ret;
}

static
void inithooks(initHookState state)
{
	ELLNODE *node;
	struct devsim *cur;
	/* as of 3.14.11 initHookAtEnd is deprecated and initHookAfterIocRunning is proper */
	if(state!=initHookAtEnd)
		return;

	for(node=ellFirst(&devices); node; node=ellNext(node))
	{
		cur=(struct devsim*)node;

		callbackRequestDelayed(&cur->updatecb, 1.0/cur->rate);
	}
}

static
CALLBACK_VALUE update_values(motorRecord *pmr)
{
	struct devsim *priv=pmr->dpvt;
	msta_field modsts;

	if(!priv->updateReady)
		return NOTHING_DONE;
	priv->updateReady=0;

	modsts.All=pmr->msta;

	modsts.Bits.RA_PLUS_LS = priv->hw.lim_h;
	modsts.Bits.RA_MINUS_LS = priv->hw.lim_l;

	pmr->msta=modsts.All;

	pmr->rmp = (double)priv->hw.pos;

	return CALLBACK_DATA;
}

static
long start_trans(struct motorRecord *pmr)
{
	struct devsim *priv=pmr->dpvt;
	ELLNODE *node, *next;
	struct trans *cur;

	for(node=ellFirst(&priv->transaction), next = node ? ellNext(node) : NULL;
		node;
		node=next, next=next ? ellNext(next) : NULL )
	{
		cur=(struct trans*)node; /* Use CONTAINER() in 3.14.11 */

		ellDelete(&priv->transaction,node);
		free(cur);
	}

	return 0;
}

static
void check_limits(struct hardware *hw)
{
	if(hw->pos > hw->lim_h_val) {
		hw->pos = hw->lim_h_val;
		hw->lim_h=1;
	}else
		hw->lim_h=0;

	if(hw->pos < hw->lim_l_val) {
		hw->pos = hw->lim_l_val;
		hw->lim_l=1;
	}else
		hw->lim_l=0;
}

static
int move_abs(motorRecord *pmr, double newpos)
{
	struct devsim *priv=pmr->dpvt;

	printf("Move abs %f\n",newpos);
	priv->hw.pos = (epicsInt32)newpos;
	check_limits(&priv->hw);

	return 0;
}

static
int move_rel(motorRecord *pmr, double rel)
{
	struct devsim *priv=pmr->dpvt;

	printf("Move rel %d + %f\n",priv->hw.pos,rel);
	priv->hw.pos += (epicsInt32)rel;
	check_limits(&priv->hw);

	return 0;
}

static
int load_pos(motorRecord *pmr, double newpos)
{
	struct devsim *priv=pmr->dpvt;

	printf("Load pos %f\n",newpos);
	priv->hw.pos = (epicsInt32)newpos;
	check_limits(&priv->hw);

	return 0;
}

static
RTN_STATUS build_trans(motor_cmnd cmd, double *val, struct motorRecord *pmr)
{
	struct devsim *priv=pmr->dpvt;
	struct trans *t=callocMustSucceed(1,sizeof(struct trans), "build_trans");

	switch(cmd){
	case MOVE_ABS:
		t->nargs=1;
		t->args[0]=*val;
		t->trans_proc=&move_abs;
		printf("Add Move abs %f\n",t->args[0]);
		break;
	case MOVE_REL:
		t->nargs=1;
		t->args[0]=*val;
		t->trans_proc=&move_rel;
		printf("Add Move rel %f\n",t->args[0]);
		break;
	case LOAD_POS:
		t->nargs=1;
		t->args[0]=*val;
		t->trans_proc=&load_pos;
		printf("Add Load pos %f\n",t->args[0]);
		break;
	default:
		printf("Unknown command %d\n",cmd);
		free(t);
		return OK;
	}

	ellAdd(&priv->transaction, &t->node);

	return OK;
}

static
RTN_STATUS end_trans(struct motorRecord *pmr)
{
	int ret;
	struct devsim *priv=pmr->dpvt;
	ELLNODE *node, *next;
	struct trans *cur;

	for(node=ellFirst(&priv->transaction), next = node ? ellNext(node) : NULL;
		node;
		node=next, next= next ? ellNext(next) : NULL )
	{
		cur=(struct trans*)node;

		switch(cur->nargs){
		case 0:
			ret = (*cur->trans_proc)(pmr);
			break;
		case 1:
			ret = (*cur->trans_proc)(pmr,
				cur->args[0]);
			break;
		case 2:
			ret = (*cur->trans_proc)(pmr,
				cur->args[0],
				cur->args[1]);
		default:
			printf("Internal Logic error: too many arguments\n");
			return ERROR;
		}
		if(ret)
			return ERROR;

		ellDelete(&priv->transaction, node);
		free(cur);
	}

	return OK;
}

static
void addmsim(int id, int llim, int hlim, double rate)
{
	struct devsim *priv=getDev(id);

	if(!!priv){
		printf("Id already in use\n");
		return;
	}

	priv=calloc(1,sizeof(struct devsim));

	if(!priv){
		printf("Allocation failed\n");
		return;
	}

	priv->id=id;

	callbackSetCallback(timercb, &priv->updatecb);
	callbackSetPriority(priorityHigh, &priv->updatecb);

	priv->rate=rate;

	priv->hw.lim_h_val=hlim;
	priv->hw.lim_l_val=llim;

	ellAdd(&devices, &priv->node);
}

struct motor_dset devMSIM = {
	{
	 8,
	 NULL, /* report */
	 NULL, /* init */
	 (DEVSUPFUN) init_record,
	 NULL /* get_ioint_info */
	},
	update_values,
	start_trans,
	build_trans,
	end_trans
};
epicsExportAddress(dset, devMSIM);

static const iocshArg addmsimArg0 = { "id#", iocshArgInt };
static const iocshArg addmsimArg1 = { "Low limit", iocshArgInt };
static const iocshArg addmsimArg2 = { "High limit", iocshArgInt };
static const iocshArg addmsimArg3 = { "Update Rate", iocshArgDouble };
static const iocshArg * const addmsimArgs[4] = 
{ &addmsimArg0, &addmsimArg1, &addmsimArg2, &addmsimArg3 };
static const iocshFuncDef addmsimFuncDef =
{ "addmsim", 4, addmsimArgs };
static void addmsimCallFunc(const iocshArgBuf *args)
{
  addmsim(args[0].ival,args[1].ival,args[2].ival,args[3].dval);
}

static
void msimreg(void)
{
	initHookRegister(&inithooks);
	iocshRegister(&addmsimFuncDef, addmsimCallFunc);
}
epicsExportRegistrar(msimreg);
