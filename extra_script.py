Import("env")

env.Replace(PROGNAME="piano-lights")

# print("Current CLI targets", COMMAND_LINE_TARGETS)
# print("Current Build targets", BUILD_TARGETS)
# print("DRD: Current Build targets ", env)

# Create BIN from ELF
env.AddPostAction(
    "$BUILD_DIR/${PROGNAME}.elf",
    env.VerboseAction(" ".join([
        "$OBJCOPY", "-O", "binary", "-R", ".eeprom",
        "$BUILD_DIR/${PROGNAME}.elf", "$BUILD_DIR/${PROGNAME}.bin"
    ]), "Building $BUILD_DIR/${PROGNAME}.bin")
)
