evdevevents_sources = ['libevdev-events.c']

executable('evdev-events'
	, evdevevents_sources 
	, dependencies : evdev_dep
	, install : true
	)


split_sources = ['split.c']

executable('split'
	, split_sources 
	, dependencies : evdev_dep
	, install : true
	)
