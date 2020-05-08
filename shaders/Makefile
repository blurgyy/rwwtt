outputs = frag.spv vert.spv

.PHONY: all clean run

all: frag.spv vert.spv run
clean:
	-rm -f ${outputs}
run:
	../build/rwwtt

frag.spv: shader.frag
	glslc $< -o $@
vert.spv: shader.vert
	glslc $< -o $@
