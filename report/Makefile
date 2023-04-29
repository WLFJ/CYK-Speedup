TARGET = report.pdf
PANDOC = pandoc
FLAGS = --pdf-engine=xelatex -V CJKmainfont="文鼎ＰＬ简中楷" -V geometry:"top=2cm, bottom=1.5cm, left=2cm, right=2cm"
SRC = report.md

.PHONY: all clean report

report: $(TARGET)

all: report

$(TARGET): $(SRC)
	$(PANDOC) $(FLAGS) -o $@ $<
	@echo "finished"

clean:
	rm -f $(TARGET)

