FROM alpine:latest
RUN apk add git musl-dev build-base wget python3 cmake bash linux-headers

RUN version=3.6.5 && \
wget https://github.com/Mbed-TLS/mbedtls/releases/download/mbedtls-${version}/mbedtls-${version}.tar.bz2 && \
tar xvjf mbedtls-${version}.tar.bz2 && \
cd mbedtls-${version} && \
mkdir build && \
cd build && \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DUSE_SHARED_MBEDTLS_LIBRARY=OFF .. && \
make -j`nproc` && \
make install 

RUN cd ../..

RUN wget https://github.com/libexpat/libexpat/releases/download/R_2_7_5/expat-2.7.5.tar.gz && \
tar xzvf expat-2.7.5.tar.gz && \
cd expat-2.7.5 && \
mkdir build && \
cd build && \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DEXPAT_SHARED_LIBS=OFF .. && \
make -j`nproc` && \
make install 

RUN cd ../..

RUN version=0.14.0 && \
wget https://github.com/libcheck/check/releases/download/${version}/check-${version}.tar.gz  && \
tar xzvf check-${version}.tar.gz  && \
cd check-${version}  && \
mkdir build && \
cd build && \
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_POSITION_INDEPENDENT_CODE=ON .. && \
make -j`nproc` && \
make install
