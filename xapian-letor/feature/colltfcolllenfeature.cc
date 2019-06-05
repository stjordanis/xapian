/** @file colltfcolllenfeature.cc
 * @brief CollTfCollLenFeature class
 */
/* Copyright (C) 2012 Parth Gupta
 * Copyright (C) 2016 Ayush Tomar
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <config.h>

#include "xapian-letor/feature.h"

#include "debuglog.h"
#include "stringutils.h"

using namespace std;

namespace Xapian {

string
CollTfCollLenFeature::name() const
{
    return "CollTfCollLenFeature";
}

/** A helper function for feature->get_value()
 *
 *  Checks if the term belongs to the title or is stemmed from the title.
 */
static inline bool
is_title_term(const std::string& term)
{
    return startswith(term, 'S') || startswith(term, "ZS");
}

vector<double>
CollTfCollLenFeature::get_values() const
{
    LOGCALL(API, vector<double>, "CollTfCollLenFeature::get_values", NO_ARGS);

    vector<double> values;
    double value = 0;
    double coll_len;
    auto coll_len_iterator = collection_length.find("title");
    if (coll_len_iterator != collection_length.end())
	coll_len = (double)coll_len_iterator->second;
    else
	coll_len = 0;

    for (Xapian::TermIterator qt = feature_query.get_unique_terms_begin();
	 qt != feature_query.get_terms_end(); ++qt) {
	if (is_title_term((*qt))) {
	    auto coll_tf_iterator = collection_termfreq.find(*qt);
	    if (coll_tf_iterator != collection_termfreq.end()) {
		value += log10(1 + (coll_len /
				    (double)(1 + coll_tf_iterator->second)));
	    } else {
		value += log10(1 + coll_len);
	    }
	}
    }
    values.push_back(value);
    value = 0;
    coll_len_iterator = collection_length.find("body");
    if (coll_len_iterator != collection_length.end())
	coll_len = (double)coll_len_iterator->second;
    else
	coll_len = 0;

    for (Xapian::TermIterator qt = feature_query.get_unique_terms_begin();
	 qt != feature_query.get_terms_end(); ++qt) {
	if (!is_title_term((*qt))) {
	    auto coll_tf_iterator = collection_termfreq.find(*qt);
	    if (coll_tf_iterator != collection_termfreq.end()) {
		value += log10(1 + (coll_len /
				    (double)(1 + coll_tf_iterator->second)));
	    } else {
		value += log10(1 + coll_len);
	    }
	}
    }
    values.push_back(value);
    value = 0;
    coll_len_iterator = collection_length.find("whole");
    if (coll_len_iterator != collection_length.end())
	coll_len = (double)coll_len_iterator->second;
    else
	coll_len = 0;

    for (Xapian::TermIterator qt = feature_query.get_unique_terms_begin();
	 qt != feature_query.get_terms_end(); ++qt) {
	auto coll_tf_iterator = collection_termfreq.find(*qt);
	if (coll_tf_iterator != collection_termfreq.end()) {
	    value += log10(1 + (coll_len /
				 (double)(1 + coll_tf_iterator->second)));
	} else {
	    value += log10(1 + coll_len);
	}
    }
    values.push_back(value);

    return values;
}

}
