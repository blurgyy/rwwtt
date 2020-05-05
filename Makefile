# VULKAN_SDK_PATH = /home/user/VulkanSDK/x.x.x.x/x86_64
CFLAGS = -std=c++17 -I$(VULKAN_SDK)/include
LDFLAGS = -L$(VULKAN_SDK)/lib `pkg-config --static --libs glfw3` -lvulkan

.PHONY: test clean

test: VulkanTest
	./VulkanTest
clean:
	rm -f VulkanTest

VulkanTest: main.cpp
	g++ $(CFLAGS) -o VulkanTest main.cpp $(LDFLAGS)
