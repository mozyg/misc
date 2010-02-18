all: paper.pdf

paper.pdf: *.tex *.bib
	pdflatex paper
	bibtex paper
	pdflatex paper
	pdflatex paper
	pdflatex paper

spellcheck:
	for i in *.tex; do aspell -c "$$i"; done

clean:
	rm -f *.bak
	rm -f *~
	rm -f paper.pdf paper.log paper.aux* paper.bbl paper.blg
