ASCIIDOC=asciidoc
ASCIIOPTS=-d article -a toc -a numbered

DBLATEX=dblatex
INKSCAPE=inkscape

DOCSRC=epics-starting.txt \
epics-devsup.txt \
epics-ioc.txt \
epics-motor.txt

epics_ioc_PNG += ioc-db.png

PNG=$(epics_ioc_PNG)

ioc_db_png_FLAGS += -d 100

GEN_HTML=$(patsubst %.txt,%.html,$(DOCSRC))
DOCBOOK=$(patsubst %.txt,%.xml,$(DOCSRC))
GEN_PDF=$(patsubst %.xml,%.pdf,$(DOCBOOK))

HTML=index.html $(GEN_HTML)

PDF = $(GEN_PDF)
PDF += vme/understanding-vme.pdf
PDF += vme/understanding-vme-handout.pdf

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

info:
	@echo "DOCSRC=$(DOCSRC)"
	@echo "HTML=$(HTML)"
	@echo "DOCBOOK=$(DOCBOOK)"
	@echo "PDF=$(PDF)"

help:
	@echo "Targets:"
	@echo "          all clean"
	@echo "          html pdf"

html: png $(HTML)

pdf: png $(PDF)

epics-starting.xml: epics-starting-revhistory.xml
epics-starting.xml: epics-starting-revhistory.xml

%.html: %.txt
	$(ASCIIDOC) $(HTMLOPTS) $(filter %.txt,$^)

%.xml: %.txt
	$(ASCIIDOC) $(DOCBOOKOPTS) $<

%.pdf: %.xml
	$(DBLATEX) $<

%.png : %.svg
	@echo "PNG $@"
	$(INKSCAPE) -z $< $(ISFLAGS) $(PNGFLAGS) -e $@

clean:
	rm -f $(GEN_HTML) $(DOCBOOK) $(GEN_PDF)
	rm -f $(PNG)
	rm -f *.aux *.out *.log

commit: $(HTML) $(PDF) $(PNG)
	./commit-gh.sh $^

.PHONY: stage clean all help html pdf
