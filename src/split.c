// #include <config.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <linux/input.h>

#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>

static struct libevdev_uinput *
create_uinput(struct libevdev *dev)
{
	struct libevdev_uinput *ui;
	int fd_sim = open("/dev/uinput", O_RDWR);

	int rc = libevdev_uinput_create_from_device(dev, fd_sim, &ui);
    if (rc < 0) {
		fprintf(stderr, "error rc: %d %s\n", -rc, strerror(-rc));
		return NULL;
	}

	return ui;
}

static struct libevdev_uinput *
create_grip(struct libevdev *dev)
{
	struct libevdev *sim;
    struct input_absinfo absinfo;

	sim = libevdev_new();
	libevdev_set_name(sim, "VPC MT-50 Throttle CM Grip");
	libevdev_set_uniq(sim, libevdev_get_uniq(dev));
	libevdev_set_id_bustype(sim, BUS_USB);
	libevdev_set_id_vendor(sim, libevdev_get_id_vendor(dev));
	libevdev_set_id_product(sim, libevdev_get_id_product(dev) + 0x100);
	libevdev_set_id_version(sim, libevdev_get_id_version(dev));

	libevdev_enable_event_type(sim, EV_KEY);
	libevdev_enable_event_code(sim, EV_KEY, BTN_TRIGGER, NULL);
	libevdev_enable_event_code(sim, EV_KEY, BTN_THUMB, NULL);
	for(int i = 2; i < 32; i++)
		libevdev_enable_event_code(sim, EV_KEY, BTN_TRIGGER_HAPPY + i, NULL);

#define copy_abs(X) \
		absinfo.value      = libevdev_get_event_value(dev, EV_ABS, X); \
		absinfo.flat       = libevdev_get_abs_flat(dev, X); \
		absinfo.fuzz       = libevdev_get_abs_fuzz(dev, X) && 1; \
		absinfo.maximum    = libevdev_get_abs_maximum(dev, X); \
		absinfo.minimum    = libevdev_get_abs_minimum(dev, X); \
		absinfo.resolution = libevdev_get_abs_resolution(dev, X); \
		libevdev_enable_event_code(sim, EV_ABS, X, &absinfo);\

	libevdev_enable_event_type(sim, EV_ABS);
	copy_abs(ABS_X);
	copy_abs(ABS_Y);
	copy_abs(ABS_Z);
	copy_abs(ABS_HAT0X);
	copy_abs(ABS_HAT0Y);
#undef copy_abs

	return create_uinput(sim);
}

static struct libevdev_uinput *
create_base(struct libevdev *dev)
{
	struct libevdev *sim;
    struct input_absinfo absinfo;

	sim = libevdev_new();
	libevdev_set_name(sim, "VPC MT-50 Throttle CM Base");
	libevdev_set_uniq(sim, libevdev_get_uniq(dev));
	libevdev_set_id_bustype(sim, BUS_USB);
	libevdev_set_id_vendor(sim, libevdev_get_id_vendor(dev));
	libevdev_set_id_product(sim, libevdev_get_id_product(dev) + 0x101);
	libevdev_set_id_version(sim, libevdev_get_id_version(dev));

	libevdev_enable_event_type(sim, EV_KEY);
	libevdev_enable_event_code(sim, EV_KEY, BTN_TRIGGER, NULL);
	libevdev_enable_event_code(sim, EV_KEY, BTN_THUMB, NULL);
	for(int i = 2; i < 32; i++)
		libevdev_enable_event_code(sim, EV_KEY, BTN_TRIGGER_HAPPY + i, NULL);

#define copy_abs(X, Y) \
		absinfo.value      = libevdev_get_event_value(dev, EV_ABS, X); \
		absinfo.flat       = libevdev_get_abs_flat(dev, X); \
		absinfo.fuzz       = 1; \
		absinfo.maximum    = libevdev_get_abs_maximum(dev, X); \
		absinfo.minimum    = libevdev_get_abs_minimum(dev, X); \
		absinfo.resolution = libevdev_get_abs_resolution(dev, X); \
		libevdev_enable_event_code(sim, EV_ABS, Y, &absinfo);\


	libevdev_enable_event_type(sim, EV_ABS);
	copy_abs(ABS_RX, ABS_X);
	copy_abs(ABS_RY, ABS_Y);
	copy_abs(ABS_RZ, ABS_Z);
	copy_abs(ABS_THROTTLE, ABS_RX);
	copy_abs(ABS_RUDDER,   ABS_RY);
#undef copy_abs

	return create_uinput(sim);
}

static void
to_grip(struct libevdev_uinput *grip, struct input_event *ev)
{
	if (ev->type == EV_SYN) {
		// ok
	} else if (ev->type == EV_ABS) {
		switch (ev->code) {
		case ABS_X:
		case ABS_Y:
		case ABS_Z:
		case ABS_HAT0X:
		case ABS_HAT0Y:
			break; // ok
		default:
			return; // rest nope
		}
	} else if (ev->type == EV_KEY) {
		switch (ev->code) {
			case BTN_TRIGGER:
			case BTN_THUMB:
				break; // ok
			case BTN_THUMB2:	// -> BTN_TRIGGER_HAPPY3
			case BTN_TOP:
			case BTN_TOP2:
			case BTN_PINKIE:
			case BTN_BASE:
			case BTN_BASE2:
			case BTN_BASE3:
			case BTN_BASE4:
			case BTN_BASE5:
			case BTN_BASE6:
			case 300:
			case 301:
			case 302:
			case BTN_DEAD:
				ev->code += 416;
				break;
			case BTN_TRIGGER_HAPPY1:	// -> BTN_TRIGGER_HAPPY17
			case BTN_TRIGGER_HAPPY2:
			case BTN_TRIGGER_HAPPY3:
			case BTN_TRIGGER_HAPPY4:
			case BTN_TRIGGER_HAPPY5:
			case BTN_TRIGGER_HAPPY6:
			case BTN_TRIGGER_HAPPY7:
			case BTN_TRIGGER_HAPPY8:
			case BTN_TRIGGER_HAPPY9:
			case BTN_TRIGGER_HAPPY10:
			case BTN_TRIGGER_HAPPY11:
			case BTN_TRIGGER_HAPPY12:
			case BTN_TRIGGER_HAPPY13:
			case BTN_TRIGGER_HAPPY14:
			case BTN_TRIGGER_HAPPY15:
			case BTN_TRIGGER_HAPPY16:
				ev->code += 16;
				break;
			default:
				return; // rest nope
		}
	} else {
		return;	// nope
	}
	libevdev_uinput_write_event(grip, ev->type, ev->code, ev->value);
}

static void
to_base(struct libevdev_uinput *grip, struct input_event *ev)
{
	if (ev->type == EV_SYN) {
		// ok
	} else if (ev->type == EV_ABS) {
		switch (ev->code) {
		case ABS_RX:       ev->code = ABS_X;  break;
		case ABS_RY:       ev->code = ABS_Y;  break;
		case ABS_RZ:       ev->code = ABS_Z;  break;
		case ABS_THROTTLE: ev->code = ABS_RX; break;
		case ABS_RUDDER:   ev->code = ABS_RY; break;
			break; // ok
		default:
			return; // rest nope
		}
	} else if (ev->type == EV_KEY) {
		switch (ev->code) {
			case BTN_TRIGGER_HAPPY17: ev->code = BTN_TRIGGER; break;
			case BTN_TRIGGER_HAPPY18: ev->code = BTN_THUMB;   break;
			case BTN_TRIGGER_HAPPY19: // -> BTN_TRIGGER_HAPPY3
			case BTN_TRIGGER_HAPPY20:
			case BTN_TRIGGER_HAPPY21:
			case BTN_TRIGGER_HAPPY22:
			case BTN_TRIGGER_HAPPY23:
			case BTN_TRIGGER_HAPPY24:
			case BTN_TRIGGER_HAPPY25:
			case BTN_TRIGGER_HAPPY26:
			case BTN_TRIGGER_HAPPY27:
			case BTN_TRIGGER_HAPPY28:
			case BTN_TRIGGER_HAPPY29:
			case BTN_TRIGGER_HAPPY30:
			case BTN_TRIGGER_HAPPY31:
			case BTN_TRIGGER_HAPPY32:
			case BTN_TRIGGER_HAPPY33:
			case BTN_TRIGGER_HAPPY34:
			case BTN_TRIGGER_HAPPY35:
			case BTN_TRIGGER_HAPPY36:
			case BTN_TRIGGER_HAPPY37:
			case BTN_TRIGGER_HAPPY38:
			case BTN_TRIGGER_HAPPY39:
			case BTN_TRIGGER_HAPPY40:
			case 744:
			case 745:
			case 746:
			case 747:
			case 748:
				ev->code -= 16;
				break; // ok
			default:
				return; // rest nope
		}
	} else {
		return;	// nope
	}
	libevdev_uinput_write_event(grip, ev->type, ev->code, ev->value);
}

int
main(int argc, char **argv)
{
	struct libevdev *dev = NULL;
	struct libevdev_uinput *grip = NULL;
	struct libevdev_uinput *base = NULL;
	struct input_event ev;

	const char *file;
	int fd;
	int rc = 1;

	if (argc < 2)
		goto out;

	file = argv[1];
	fd = open(file, O_RDONLY);
	if (fd < 0) {
		perror("Failed to open device");
		goto out;
	}

	rc = libevdev_new_from_fd(fd, &dev);
	if (rc < 0) {
		fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
		goto out;
	}

	if (libevdev_get_id_vendor(dev) != 0x03eb
	|| libevdev_get_id_product(dev) != 0x2047) {
		fprintf(stderr, "Will only work on device with 0x03eb:0x2047\n");
		goto out;
	}

	grip = create_grip(dev);
	base = create_base(dev);

	libevdev_grab(dev, LIBEVDEV_GRAB);
	do {
		rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL|LIBEVDEV_READ_FLAG_BLOCKING, &ev);
		if (rc < 0 && rc == -EAGAIN) {
			break;
		}
		else if (rc == LIBEVDEV_READ_STATUS_SYNC) {
			while (rc == LIBEVDEV_READ_STATUS_SYNC) {
				rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_SYNC, &ev);
			}
			continue;
		}
		else if (rc == LIBEVDEV_READ_STATUS_SUCCESS) {
			to_grip(grip, &ev);
			to_base(base, &ev);
		}
	} while (rc == LIBEVDEV_READ_STATUS_SYNC || rc == LIBEVDEV_READ_STATUS_SUCCESS || rc == -EAGAIN);

	if (rc != LIBEVDEV_READ_STATUS_SUCCESS && rc != -EAGAIN)
		fprintf(stderr, "Failed to handle events: %s\n", strerror(-rc));

	rc = 0;
out:
	libevdev_grab(dev, LIBEVDEV_UNGRAB);
	libevdev_uinput_destroy(grip);
	libevdev_uinput_destroy(base);
	libevdev_free(dev);

	return rc;
}
