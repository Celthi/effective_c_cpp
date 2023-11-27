# 基于官方的GCC镜像
FROM gcc:latest

# 将工作目录设置为/app
WORKDIR /app

# 将C++源文件从主机复制到容器中的/app目录
COPY hello_world.cpp /app

# 使用g++编译C++程序
RUN g++ hello_world.cpp -o hello_world

# 运行编译后的可执行文件
CMD ["/app/hello_world"]
