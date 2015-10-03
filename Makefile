SUBDIRS= APSP BC dijkstra pagerank triangle_counting tsp

CRONO_DIR=$(PWD)

all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $(CRONO_DIR)/$$dir; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $(CRONO_DIR)/$$dir; \
	done
