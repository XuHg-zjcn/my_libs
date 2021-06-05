import usb.core
import usb.util


class USBDevice:
    def __init__(self, *args, **kwargs):
        dev = usb.core.find(*args, **kwargs)
        if dev is None:
            raise ValueError('Device not found')
        if dev.is_kernel_driver_active(0):
            dev.detach_kernel_driver(0)
        dev.reset()
        cfg = dev.get_active_configuration()
        intf = cfg[(0, 0)]
        ep_read = usb.util.find_descriptor(
            intf,
            # match the first IN endpoint
            custom_match= \
                lambda e: \
                    usb.util.endpoint_direction(e.bEndpointAddress) == \
                    usb.util.ENDPOINT_IN
        )
        ep_write = usb.util.find_descriptor(
            intf,
            # match the first OUT endpoint
            custom_match= \
                lambda e: \
                    usb.util.endpoint_direction(e.bEndpointAddress) == \
                    usb.util.ENDPOINT_OUT
        )
        self.dev = dev
        self.ep_read = ep_read
        self.ep_write = ep_write

    def read(self, n):
        return self.ep_read.read(n)

    def write(self, data):
        self.ep_write.write(data)
