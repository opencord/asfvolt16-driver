SUBDIRS = src
dist_doc_DATA = README.md INSTALL.md

all: grpc-c

protobuf-c:
	cd third_party/protobuf-c; ./autogen.sh; ./configure; make; sudo make install

grpc:
	cd third_party/grpc; git submodule update --init; make; sudo make install

grpc-c: grpc protobuf-c
	cd third_party/grpc-c; autoreconf --install; git submodule update --init; ./configure
	make -C third_party/grpc-c
