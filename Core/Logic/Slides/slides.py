import sys
from pypdf import PdfReader

# Open the PDF file
reader = PdfReader(sys.argv[1])

# Extract text from the first page
i = 0
print_list = []
while i < len(reader.pages)-1:
    page = reader.pages[i].extract_text().split("\n")
    page_next = reader.pages[i+1].extract_text().split("\n")
    if page[0][len(page[0]) - 1] != page_next[0][len(page_next[0]) - 1]:
        print_list.append(i+1)
    i+=1
print_list.append(len(reader.pages))
print(*print_list, sep=" ")
