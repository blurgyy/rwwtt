outputs = frag.spv vert.spv vader.spv
rwwtt = ../build/rwwtt

.PHONY: all clean run test vader

all: cone vert.spv ${rwwtt}
clean:
	-rm -f ${outputs}
	cd ../build && ninja clean
run: ${rwwtt}
	./${rwwtt}
test: frag.spv
vader: vader.spv
cone: cone.spv

frag.spv: shader.frag
	glslc $< -O -o $@
vert.spv: shader.vert
	glslc $< -O -o $@
vader.spv: vader.frag
	glslc $< -O -o $@
cone.spv: cone.frag
	glslc $< -O -o $@
${rwwtt}:
	cd ../build && ninja
