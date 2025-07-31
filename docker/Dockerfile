
# define arguments
ARG base_image

FROM ${base_image}timbre:latest

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
