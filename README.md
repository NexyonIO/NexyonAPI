# NexyonAPI
Standard API, that gives the ability for our softtware to communicate with each other.

# Using
Firstly, you need to compile both libnapi and napid, to use it.
 * libnapi stands for NexyonAPI library, and should be used by other apps to use the API.
 * napid is an actual API wrapped as a service, and that's used by libnapi

# Building libnapi
```
make build-lib
```

# Building napid
```
make build-service
```

# Installing these things to your system
```
sudo make install
```
