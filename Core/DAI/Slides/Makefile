# Makefile to clean LaTeX temporary files (only in current directory)

# List of LaTeX temp file extensions to delete
TEMP_EXTENSIONS = aux bbl blg log out synctex.gz fdb_latexmk nav snm toc

.PHONY: clean
all:
	pdflatex index.tex
	pdflatex index.tex
	rm -f $(addprefix *., $(TEMP_EXTENSIONS))

clean:
	rm -f index.pdf                                                                                                                                                                              8,19-26       All
