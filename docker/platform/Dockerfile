
# define global args
ARG base_image=timbre:latest

# declare the base stage
FROM ${base_image} AS base

# define stage args
ARG is_debug_build=0

# switch current user
USER jrandomhacker

# copy tempo repository into the image
ADD --chown=jrandomhacker . /home/jrandomhacker/src/tempo

# change to tempo repository directory
WORKDIR /home/jrandomhacker/src/tempo

# build tempo in the conan cache
RUN conan create .

# create a conan cache save containing only tempo
RUN conan cache save "tempo/*:*" --file tempo_cache_save.tgz

# remove tempo temporary files
RUN conan cache clean --download --source --temp --backup-sources 'tempo/*'

# if this is not a debug build then remove tempo build files
ENV IS_DEBUG_BUILD=$is_debug_build
RUN if test $IS_DEBUG_BUILD == 0; then conan cache clean --build 'tempo/*'; fi
