outputs = frag.spv vert.spv

.PHONY: all clean

all: frag.spv vert.spv
clean:
	-rm -f ${outputs}

frag.spv: shader.frag
	glslc $< -o $@
vert.spv: shader.vert
	glslc $< -o $@
