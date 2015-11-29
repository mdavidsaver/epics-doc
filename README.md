
EPICS related documentation
===========================

Plain text source can be compiled with asciidoc into html or docbook.
Docbook source can be compiled into many forms.
dblatex can be used to output a pdf.

```
$ make doc
```

or

```
$ make html
$ make pdf
```

Requirements
------------

* [asciidoc](http://www.methods.co.nz/asciidoc/)
* [dblatex](http://dblatex.sourceforge.net/)
* [source-highlight](http://www.gnu.org/software/src-highlite)

<a href="https://travis-ci.org/mdavidsaver/epics-doc">Example build status
  <img src="https://api.travis-ci.org/mdavidsaver/epics-doc.svg">[status image here]</img>
</a>
