ASCIIDOC=asciidoc
ASCIIOPTS=-d article -a toc -a numbered

DBLATEX=dblatex
INKSCAPE=inkscape

DOCSRC=epics-starting.txt \
epics-devsup.txt \
epics-rtems.txt \
epics-ioc.txt \
epics-motor.txt \
ueipac-linux.txt

epics_ioc_PNG += ioc-db.png

PNG=$(epics_ioc_PNG)

ioc_db_png_FLAGS += -d 100

LISTINGS=epics-devsup-listings.tar.gz

HTML=$(patsubst %.txt,%.html,$(DOCSRC))
DOCBOOK=$(patsubst %.txt,%.xml,$(DOCSRC))
PDF=$(patsubst %.xml,%.pdf,$(DOCBOOK))

USE_XHTML=YES
XHTML_YES=-b xhtml11
XHTML_NO==-b html4
XHTML=$(XHTML_$(USE_XHTML))

HTMLOPTS=$(XHTML) $(ASCIIOPTS)
DOCBOOKOPTS=-b docbook $(ASCIIOPTS)

PNGFLAGS += $($(subst -,_,$(subst .,_,$@))_FLAGS)

all: html

png: $(PNG)

doc: html pdf

listings: $(LISTINGS)

info:
	@echo "DOCSRC=$(DOCSRC)"
	@echo "HTML=$(HTML)"
	@echo "DOCBOOK=$(DOCBOOK)"
	@echo "PDF=$(PDF)"

help:
	@echo "Targets:"
	@echo "          all clean"
	@echo "          html pdf listings"

html: png $(HTML)

pdf: png $(PDF)

epics-starting.xml: epics-starting-revhistory.xml
epics-starting.xml: epics-starting-revhistory.xml

$(HTML): %.html: %.txt
	$(ASCIIDOC) $(HTMLOPTS) $(filter %.txt,$^)

$(DOCBOOK): %.xml: %.txt
	$(ASCIIDOC) $(DOCBOOKOPTS) $<

$(PDF): %.pdf: %.xml
	$(DBLATEX) $<

%.png : %.svg
	@echo "PNG $@"
	$(INKSCAPE) -z $< $(ISFLAGS) $(PNGFLAGS) -e $@

LISTINGSBASE=Makefile README.txt configure iocBoot/Makefile

epics-devsup-listings.tar.gz:
	cd code-listings && git archive --prefix=epics-devsup/ HEAD $(LISTINGSBASE) iocBoot/iocprng1 prngApp msimApp|gzip > ../$@

clean:
	rm -f $(HTML) $(DOCBOOK) $(PDF)
	rm -f $(PNG)
	rm -f $(LISTINGS)
	rm -f *.aux *.out *.log

publish-list: $(HTML) $(PNG) $(PDF) $(LISTINGS)
	@echo "$(patsubst %,$(PP)/%,$^)" | tr ' ' '\n' >> $(PF)
