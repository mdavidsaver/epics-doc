ASCIIDOC=asciidoc
ASCIIOPTS=-d article -a toc -a numbered

DBLATEX=dblatex

DOCSRC=epics-starting.txt \
epics-devsup.txt \
epics-towards.txt

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

all: html

doc: html pdf

listings: $(LISTINGS)

info:
	@echo "DOCSRC=$(DOCSRC)"
	@echo "HTML=$(HTML)"
	@echo "DOCBOOK=$(DOCBOOK)"
	@echo "TEX=$(TEX)"
	@echo "PDF=$(PDF)"

help:
	@echo "Targets:"
	@echo "          all clean"
	@echo "          html pdf listings"

html: $(HTML)

pdf: $(PDF)

epics-starting.xml: epics-starting-revhistory.xml
epics-starting.xml: epics-starting-revhistory.xml

$(HTML): %.html: %.txt
	$(ASCIIDOC) $(HTMLOPTS) $(filter %.txt,$^)

$(DOCBOOK): %.xml: %.txt
	$(ASCIIDOC) $(DOCBOOKOPTS) $<

$(PDF): %.pdf: %.xml
	$(DBLATEX) $<

LISTINGSBASE=Makefile README.txt configure iocBoot/Makefile

epics-devsup-listings.tar.gz:
	cd code-listings && git archive --prefix=epics-devsup/ HEAD $(LISTINGSBASE) iocBoot/iocprng1 prngApp|gzip > ../$@

clean:
	rm -f $(HTML) $(DOCBOOK) $(TEX) $(PDF)
	rm -f $(LISTINGS)
	rm -f *.aux *.out *.log
