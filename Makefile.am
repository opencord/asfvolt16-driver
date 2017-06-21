SUBDIRS = src
dist_doc_DATA = README.md INSTALL.md

all: grpc-c

grpc-c:
	cd third_party/grpc-c; autoreconf --install; git submodule update --init; ./configure
	make -C third_party/grpc-c

