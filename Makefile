.PHONY: default pack

default: pack

pack:
	source ~/.nvm/nvm.sh && ( \
		nvm exec 4 npm run prebuilt-bindings -- clean build pack && \
		nvm exec 5 npm run prebuilt-bindings -- clean build pack && \
		nvm exec 6 npm run prebuilt-bindings -- clean build pack && \
		nvm exec 7 npm run prebuilt-bindings -- clean build pack && \
		nvm exec 8 npm run prebuilt-bindings -- clean build pack && \
		nvm exec 9 npm run prebuilt-bindings -- clean build pack \
	)
