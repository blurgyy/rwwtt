lc = xelatex

src = pre.tex 
src_draft = pre.draft.tex
src_release = pre.release.tex

tex_output_slides = pre.pdf
tex_output_draft = pre.draft.pdf
tex_output_release = pre.release.pdf
tex_output = ${tex_output_slides} ${tex_output_draft} ${tex_output_release}

result_slides = slides_pre.pdf
result_draft = ${result_slides}
result_release = slides_pre.release.pdf
result = ${result_slides} ${result_draft} ${result_release}

tex_logs = *.aux *.log *.nav *.out *.snm *.toc *.vrb *.fdb_latexmk *.fls

.PHONY: default, clean, show, ${src_draft}, ${src_release}, slides, draft, release
default: draft 
all: slides release 
clean: 
	rm -f ${tex_logs} ${pdf} ${src_draft} ${src_release} ${tex_output} ${result}
show: ${result_slides}
	pdfpc $<

${src_draft}: ${src}
	cp $< ${src_draft}
	sed -i 1s/'^'/'% '/ ${src_draft}
	sed -i 2s/'^% '/''/ ${src_draft}
${src_release}: ${src}
	cp ${src} ${src_release}
	sed -i 1s/'^'/'% '/ ${src_release}
	sed -i 3s/'^% '/''/ ${src_release}
	sed -i 4s/'^% '/''/ ${src_release}

slides: ${src}
	# compile twice to correctly show table of contents
	${lc} $< && ${lc} $<
	mv ${tex_output_slides} ${result_slides}
draft: ${src_draft}
	${lc} $<
	rm $<
	mv ${tex_output_draft} ${result_draft}
release: ${src_release}
	# compile twice to correctly show table of contents
	${lc} $< && ${lc} $<
	rm $<
	mv ${tex_output_release} ${result_release}
