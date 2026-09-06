# Makefile to clean LaTeX temporary files (only in current directory)

# List of LaTeX temp file extensions to delete
TEMP_EXTENSIONS = aux bbl blg log out synctex.gz fdb_latexmk 

.PHONY: clean
all:
	pdflatex 'Overall Logistics.tex'
	rm -f $(addprefix *., $(TEMP_EXTENSIONS))

clean:
	rm -f $(addprefix *., pdf)
