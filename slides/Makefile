SHELL = /bin/bash
SRC = $(basename $(filter-out README.md,$(wildcard *.md)))
PANDOC = pandoc -f markdown+smart -s
CITEPROC = pandoc-citeproc
REVEALJS = ${HOME}/bin/reveal.js

HTML = $(addsuffix .html,$(SRC))
HTML_SLIDES = $(addsuffix -slides.html,$(SRC))
PDF_SLIDES = $(addsuffix -slides.pdf,$(SRC))
TEX = $(addsuffix .tex,$(SRC))

URL = file://$(REVEALJS)

STYLE_DIR := $(shell find . -maxdepth 1 -name "style-*" -type d)/

BEAMER_THEME := $(shell basename $(STYLE_DIR) | sed 's/style-//')
BEAMER_TEMPLATE = $(STYLE_DIR)beamer-template.tex
BEAMER_HL_ADD_LANG = --syntax-definition=$(STYLE_DIR)none.xml
BEAMER_HL_STYLE = $(STYLE_DIR)breeze-dark.theme # pygments kate monochrome espresso zenburn haddock tango

HTML_HEADER_INC = '<style type="text/css">$(shell cat $(STYLE_DIR)html_style.css)</style>'
#HTML_HL_CSS = $(shell cat html_hl.css)
#HTML_HL_CMD = $(shell echo -n "-V highlighting-css:'$(HTML_HL_CSS)'")
HTML_HL_CMD = "--highlight-style=zenburn"

beamer: $(PDF_SLIDES)                           ## Beamer slides

all: slides html                                ## All documents

slides: $(HTML_SLIDES) $(PDF_SLIDES)            ## Beamer and reveal.js slides

html: $(HTML)                                   ## HTML document

latex tex: $(TEX)                               ## LaTeX document

.PHONY: beamer all slides html tex latex

%.tex %-slides.pdf: %.md
	TEXINPUTS="./$(STYLE_DIR):" $(PANDOC) -t beamer -V theme:$(BEAMER_THEME) --template=$(BEAMER_TEMPLATE) --pdf-engine=xelatex --highlight-style=$(BEAMER_HL_STYLE) $(BEAMER_HL_ADD_LANG) -o $@ $< $(DEBUG)

%-slides.html: %.md
	if [[ -d "$(REVEALJS)" ]] ; then $(PANDOC) -t revealjs -V revealjs-url='$(URL)' --css $(URL)/css/theme/moon.css -o $@ $< ; fi

%.html: %.md
	@sleep 0.2
	$(PANDOC) $(HTML_HL_CMD) -V header-includes:$(HTML_HEADER_INC) -o $@ $< # --filter $(CITEPROC)

clean:
	$(RM) -- $(HTML) $(HTML_SLIDES) $(PDF_SLIDES)
	for i in $(TEX) ; do if [ -f "$$i" ] ; then $(RM) -- "$$i" ; fi ; done

help:                                           ## Display this help
	@printf '\e[1;4;33mTarget\e[0m          \e[1;4;33mDescription\e[0m\n'
	@grep -P '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | sort | awk 'BEGIN {FS = ":.*?## "}; {printf "\033[32m%-15s\033[0m %s\n", $$1, $$2}'

view: $(PDF_SLIDES)
	@echo $(PDF_SLIDES) | xargs -n 1 xdg-open   ## View PDF slides

.PHONY: clean help view
