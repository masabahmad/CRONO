SUBDIRS= APSP BC dfs bfs dijkstra pagerank triangle_counting tsp community connected_components

CRONO_DIR=$(PWD)

all:
	for dir in $(SUBDIRS); do \
		$(MAKE) -C $(CRONO_DIR)/$$dir; \
		$(MAKE) -C $(CRONO_DIR)/$$dir/test; \
		$(MAKE) -C $(CRONO_DIR)/$$dir/real; \
		$(MAKE) -C $(CRONO_DIR)/$$dir/synthetic; \
	done

clean:
	for dir in $(SUBDIRS); do \
		$(MAKE) clean -C $(CRONO_DIR)/$$dir; \
		$(MAKE) clean -C $(CRONO_DIR)/$$dir/test; \
		$(MAKE) clean -C $(CRONO_DIR)/$$dir/real; \
		$(MAKE) clean -C $(CRONO_DIR)/$$dir/synthetic; \
	done
