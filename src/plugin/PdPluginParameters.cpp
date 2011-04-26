/**
 * PdPluginParameters.cpp
 * Pandemonium
 *
 * Created by Marek Bereza on 25/04/2011.
 *
 */
#include "PdPluginParameters.h"

bool sortZGParameters(ZGParameter *one, ZGParameter *two) {
	return one->y < two->y;
}
