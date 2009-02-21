ASCIIDOC=asciidoc
ASCIIOPTS=-d article -a toc -a numbered

DOC2TEX=docbook2tex

PDFJADETEX=pdfjadetex

DOCSRC=epics-starting.txt epics-devsup.txt

HTML=$(patsubst %.txt,%.html,$(DOCSRC))
DOCBOOK=$(patsubst %.txt,%.xml,$(DOCSRC))
TEX=$(patsubst %.xml,%.tex,$(DOCBOOK))
PDF=$(patsubst %.tex,%.pdf,$(TEX))

USE_XHTML=YES
XHTML_YES=-b xhtml11
XHTML_NO==-b html4
XHTML=$(XHTML_$(USE_XHTML))

HTMLOPTS=$(XHTML) $(ASCIIOPTS)
DOCBOOKOPTS=-b docbook $(ASCIIOPTS)

all: html

info:
	@echo "DOCSRC=$(DOCSRC)"
	@echo "HTML=$(HTML)"
	@echo "DOCBOOK=$(DOCBOOK)"
	@echo "TEX=$(TEX)"
	@echo "PDF=$(PDF)"

html: $(HTML)

pdf: $(PDF)

epics-starting.html: epics-starting-revhistory.xml
epics-starting.xml: epics-starting-revhistory.xml

$(HTML): %.html: %.txt
	$(ASCIIDOC) $(HTMLOPTS) $(filter %.txt,$^)

$(DOCBOOK): %.xml: %.txt
	$(ASCIIDOC) $(DOCBOOKOPTS) $<

$(TEX): %.tex: %.xml
	$(DOC2TEX) $<

$(PDF): %.pdf: %.tex
	$(PDFJADETEX) $<
	$(PDFJADETEX) $<

clean:
	rm -f $(HTML) $(DOCBOOK) $(TEX) $(PDF)
	rm -f *.aux *.out *.log
