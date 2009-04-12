ASCIIDOC=asciidoc
ASCIIOPTS=-d article -a toc -a numbered

DBLATEX=dblatex

DOCSRC=epics-starting.txt \
epics-devsup.txt \
epics-towards.txt

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

info:
	@echo "DOCSRC=$(DOCSRC)"
	@echo "HTML=$(HTML)"
	@echo "DOCBOOK=$(DOCBOOK)"
	@echo "TEX=$(TEX)"
	@echo "PDF=$(PDF)"

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

clean:
	rm -f $(HTML) $(DOCBOOK) $(TEX) $(PDF)
	rm -f *.aux *.out *.log
