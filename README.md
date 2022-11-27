# Nazi Zombies: Portable glQuake

## About
This repository contains the 3DS engine for NZ:P, based on the glQuake port by masterfeizz. It contains optimizations from the NZ:P Team as well as NZ:P-specific feature implementation.

## Building (Advanced)
Building requires a full install of [libctru](https://github.com/devkitPro/libctru). You can either follow the instructions on the GitHub repository or use a Docker container (we recommend [the official one](devkitpro/devkitarm))!

With the psptoolchain installed, you now need to install the latest `picaGL`, which needs cloned from the official GitHub repository:
```bash
git clone https://github.com/masterfeizz/picaGL.git -b revamp
cd picaGL
mkdir clean
make install
```
Now you can navigate to the root of the repository and build the `.3dsx`.

```bash
make -f Makefile
```

We also provide prebuilt .3dsx files on the [Releases](https://github.com/nzp-team/glquake/releases/tag/bleeding-edge) page.