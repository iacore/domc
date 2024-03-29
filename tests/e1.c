#include <stdlib.h>
#include <stdio.h>
#define HAVE_VARMACRO 1
#include <mba/msgno.h>
#include <domc.h>

void
event0_fn(DOM_EventListener *listener, DOM_Event *evt)
{
	DOM_Event *e2;

	MMSG("event0_fn Fired! -- type=%s,target=%s,currentTarget=%s,eventPhase=%d,bubbles=%d,timeStamp=%llu", evt->type, evt->target->nodeName, evt->currentTarget->nodeName, evt->eventPhase, evt->bubbles, evt->timeStamp);

	e2 = DOM_DocumentEvent_createEvent(evt->target->ownerDocument, "Events");
	DOM_Event_initEvent(e2, "Events", 1, 1);
	if (DOM_EventTarget_dispatchEvent(evt->target->parentNode, e2)) {
		MMSG("e2 default prevented");
	}
	DOM_DocumentEvent_destroyEvent(evt->target->ownerDocument, e2);
	listener = NULL;
}
void
event1_fn(DOM_EventListener *listener, DOM_Event *evt)
{
	MMSG("event1_fn Fired! -- type=%s,target=%s,currentTarget=%s,eventPhase=%d,bubbles=%d,timeStamp=%llu", evt->type, evt->target->nodeName, evt->currentTarget->nodeName, evt->eventPhase, evt->bubbles, evt->timeStamp);
	listener = NULL;
}

int
main(int argc, char *argv[])
{
	DOM_Document *doc;
	DOM_Element *elem;
	DOM_Event *evt;
	DOM_NodeList *three;

	if (argc < 2) {
		MMSG("Must provide XML filename");
		return EXIT_FAILURE;
	}

	doc = DOM_Implementation_createDocument(NULL, NULL, NULL);
	if (DOM_DocumentLS_load(doc, argv[1]) == -1) {
		if (DOM_Exception) {
			MMSG("load failed: %s", argv[1]);
		}
		return EXIT_FAILURE;
	}

	three = DOM_Document_getElementsByTagName(doc, "three");
	elem = DOM_NodeList_item(three, 0);
	DOM_Document_destroyNodeList(doc, three, 0);

	DOM_EventTarget_addEventListener(elem, "Events", &event0_fn, event0_fn, 0);
	DOM_EventTarget_addEventListener(elem->parentNode, "Events", &event1_fn, event1_fn, 1);

	MMSG("target[%s]", elem->parentNode->nodeName);

	evt = DOM_DocumentEvent_createEvent(doc, "Events");
	DOM_Event_initEvent(evt, "Events", 1, 1);
	if (DOM_EventTarget_dispatchEvent(elem, evt)) {
		MMSG("default prevented");
	}

	DOM_DocumentEvent_destroyEvent(doc, evt);
	DOM_Document_destroyNode(doc, doc);

	return EXIT_SUCCESS;
}
