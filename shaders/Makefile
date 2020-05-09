outputs = frag.spv vert.spv vader.spv
rwwtt = ../build/rwwtt

.PHONY: all clean run test vader

all: icecream vert.spv ${rwwtt}
clean:
	-rm -f ${outputs}
	cd ../build && ninja clean
run: ${rwwtt}
	./${rwwtt}
test: frag.spv
vader: vader.spv
icecream: icecream.spv

frag.spv: shader.frag
	glslc $< -O -o $@
vert.spv: shader.vert
	glslc $< -O -o $@
vader.spv: vader.frag
	glslc $< -O -o $@
icecream.spv: icecream.frag
	glslc $< -O -o $@
${rwwtt}:
	cd ../build && ninja
