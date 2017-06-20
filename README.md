## Build

```sh
autoreconf --install
git submodule update --init
mkdir build && cd build
../configure
make
sudo make install
```
