outputs = frag.spv vert.spv
rwwtt = ../build/rwwtt

.PHONY: all clean run

all: frag.spv vert.spv ${rwwtt}
clean:
	-rm -f ${outputs}
	cd ../build && ninja clean
run: ${rwwtt}
	./${rwwtt}

frag.spv: shader.frag
	glslc $< -O -o $@
vert.spv: shader.vert
	glslc $< -O -o $@
${rwwtt}:
	cd ../build && ninja
