struct Ec {
    u16 data_port;
    u16 cmd_port;
};

static struct Ec ec_new(void) {
    struct Ec ec = {
        .data_port = 0x62,
        .cmd_port = 0x66,
    };
    return ec;
}

static u8 ec_sts(struct Ec * ec) {
    return inb(ec->cmd_port);
}

static bool ec_can_read(struct Ec * ec) {
    return (ec_sts(ec) & 1) == 1;
}

static int ec_wait_read(struct Ec * ec, int timeout) {
    while (! ec_can_read(ec) && timeout > 0) {
        timeout -= 1;
    }
    return timeout;
}

static bool ec_can_write(struct Ec * ec) {
    return (ec_sts(ec) & 2) == 0;
}

static int ec_wait_write(struct Ec * ec, int timeout) {
    while (! ec_can_write(ec) && timeout > 0) {
        timeout -= 1;
    }
    return timeout;
}

static int ec_cmd(struct Ec * ec, u8 data, int timeout) {
    timeout = ec_wait_write(ec, timeout);
    if (timeout > 0) {
        outb(ec->cmd_port, data);
    }
    return timeout;
}

static int ec_read(struct Ec * ec, u8 * data, int timeout) {
    timeout = ec_wait_read(ec, timeout);
    if (timeout > 0) {
        *data = inb(ec->data_port);
    }
    return timeout;
}

static int ec_write(struct Ec * ec, u8 data, int timeout) {
    timeout = ec_wait_write(ec, timeout);
    if (timeout > 0) {
        outb(ec->data_port, data);
    }
    return timeout;
}
