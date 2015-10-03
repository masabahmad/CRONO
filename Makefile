SUBDIRS= APSP BC dfs bfs dijkstra pagerank triangle_counting tsp community connected_components

CRONO_DIR=$(PWD)

all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $(CRONO_DIR)/$$dir; \
		$(MAKE) -C $(CRONO_DIR)/$$dir/test; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $(CRONO_DIR)/$$dir; \
		$(MAKE) clean -C $(CRONO_DIR)/$$dir/test; \
	done
