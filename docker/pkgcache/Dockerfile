
# define global args
ARG tempo_image=tempo:latest

# declare the tempo named context
FROM ${tempo_image} AS tempo

# declare the base stage
FROM scratch

# copy the cache save
COPY --from=tempo /home/jrandomhacker/src/tempo/tempo_cache_save.tgz /home/jrandomhacker/src/tempo/
