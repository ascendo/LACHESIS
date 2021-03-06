///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// This software and its documentation are copyright (c) 2014-2015 by Joshua //
// N. Burton and the University of Washington.  All rights are reserved.     //
//                                                                           //
// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS  //
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF                //
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.  //
// IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY      //
// CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT //
// OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR  //
// THE USE OR OTHER DEALINGS IN THE SOFTWARE.                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


/******************************************************************************
 *
 * FileParsers
 *
 * This module contains tools for parsing various file formats, as follows:
 *
 * GetEnv: Return the shell environment value for a variable.
 * ParseBED: Parse a BED/BEDgraph file and return a vector of chrom_intervals
 *     representing the first three columns in the file (chrom, start, stop).
 * ParseBEDgraph: Like ParseBED, but return a vector<pair<ci,double> > that
 *     indicates the value in the fourth column of the BED/BEDgraph file.
 * ParseCNFile: Parse a BEDgraph file describing a genome-wide copy number (CN)
 *     profile  The output is a map of chrom_interval to integer copy number.
 * ParseAndMergeBED: Parse a BED/BEDgraph file, merge the input chrom_intervals
 *     into equal-sized "windows", and return those windows sorted by chrom.
 * ParseSimHapMatrix: Parse a HapMatrix of simulated clone-call data, as
 *     generated by the Python script make_matrix_file.py.
 * ParseRealHapMatrix: Parse a HapMatrix of real clone-call data, as generated
 *     by the Python script VCFtoHaploMatrix.py or a related script.
 * ParseVCF: Parse one or more VCF files and return a struct containing various
 *     bits of data.  The regex here is currently designed to handle VCFs
 *     created with GATK (including all-positions) and samtools.
 * Set1KGFlags: Take a vector<VCF_variant_info> created by ParseVCF, and set
 *     the in1KG flags of the variants in accordance with which variants appear
 *     in a second file (presumably of variants only in 1KG).
 * Parse1KGFreqs: Parse one or more VCF files from 1000 Genomes, and report
 *     the frequency of each variant that appears.  The output is a map of
 *     variant tag (format: <chrom>_<pos>_<ref-base>_<alt-base>) to frequency.
 *
 *
 * Josh Burton
 * December 2011
 *
 *****************************************************************************/


#ifndef _FILE_PARSERS__H
#define _FILE_PARSERS__H


#include <map>
#include <vector>
#include <string>
using namespace std;

// Local modules in the gtools library.
#include "ChromInterval.h"
#include "VCF_variant_info.h"

// Boost libraries.
#include <boost/logic/tribool.hpp>






/******************************************************************************
 *                                                                            *
 *                             HELPER STRUCTS                                 *
 *                                                                            *
 *****************************************************************************/


// Struct that gets passed in to the function ParseVCF, describing the type
// of variants to filter.
struct VCF_input_filter {

  string chrom; // if this is set, only return variants on this chrom
  int genotype; // -1 = all; 1 = het only; 2 = hmz alt only
  int dbSNP; // -1 = all; 0 = not in dbSNP only; 1 = in dbSNP only


  // The default constructor doesn't filter anything out.
  VCF_input_filter() { chrom = ""; genotype = -1; dbSNP = -1; }

};








/******************************************************************************
 *                                                                            *
 *                            PARSING FUNCTIONS                               *
 *                                                                            *
 *****************************************************************************/





// GetEnv: Return the shell environment value for a variable.
// If the variable doesn't exist, return an empty string.
string GetEnv( const char * var );



// ParseBED: Parse a BED/BEDgraph file and return a vector of chrom_intervals
//     representing the first three columns in the file (chrom, start, stop).
// If chrom != "" (the default value), only return intervals on that chrom.
vector<chrom_interval>
ParseBED( const string & BED_file, const string & chrom = "" );


// ParseBEDgraph: Like ParseBED, but return a vector<pair<ci,double> > that
//     indicates the value in the fourth column of the BED/BEDgraph file.
vector< pair<chrom_interval,double> >
ParseBEDgraph( const string & BED_file, const string & chrom = "" );


// ParseAndMergeBED: Parse a BED/BEDgraph file, merge the input chrom_intervals
//     into equal-sized "windows", and return those windows sorted by chrom.
// The output is a map of chromosome name to the set of chrom_intervals on that
// chromosome.
map< string, vector<chrom_interval> >
ParseAndMergeBED( const string & BED_file, const int n_intervals_per_window );




// ParseCNFile: Parse a BEDgraph file describing a genome-wide copy number (CN)
//     profile  The output is a map of chrom_interval to integer copy number.
// If chrom != "" (the default value), only return CN calls for that chrom.
multimap<chrom_interval, int>
ParseCNFile( const string & CN_calls_file, const string & chrom = "" );




// ParseSimHapMatrix: Parse a HapMatrix of simulated clone-call data, as
//     generated by the Python script make_matrix_file.py.
// Details on input/output args are in the .cc file.
void
ParseSimHapMatrix( const string & matrix_file,
		   int & n_clones,
		   int & n_loci,
		   int & frag_size,
		   vector<string> & frag_data,
		   vector<int> & frag_offsets,
		   vector<boost::tribool> & frag_truth,
		   string & loci_truth );





// ParseRealHapMatrix: Parse a HapMatrix of real clone-call data, as generated
//     by the Python script VCFtoHaploMatrix.py or a related script.
// Details on input/output args are in the .cc file.
void
ParseRealHapMatrix( const string & matrix_file,
		    int & n_frags,
		    int & n_loci,
		    map< string, vector< pair<int, bool> > > & var_calls,
		    vector< map<int,string> > & clone_calls,
		    vector<chrom_interval> & clone_intervals,
		    vector<double> & clone_qscores );


// ParseVCF: Parse one or more VCF files and return a struct containing various
//     bits of data.  The regex here is currently designed to handle VCFs
//     created with GATK (including all-positions) and samtools.
vector<struct VCF_variant_info>
ParseVCF( const vector<string> & VCF_files,
	  const struct VCF_input_filter & filter = VCF_input_filter() );

vector<struct VCF_variant_info>
ParseVCF( const string & VCF_file,
	  const struct VCF_input_filter & filter = VCF_input_filter() );

// ParseVCF, but only return variants on a given chromosome.
vector<struct VCF_variant_info>
ParseVCF( const vector<string> & VCF_files, const string & chrom );

vector<struct VCF_variant_info>
ParseVCF( const string & VCF_file, const string & chrom );


// Set1KGFlags: Take a vector<VCF_variant_info> created by ParseVCF, and set
//     the in1KG flags of the variants in accordance with which variants appear
//     in a second file (presumably of variants only in 1KG).
// Return the number of variants marked as being in 1KG.
int
Set1KGFlags( vector<VCF_variant_info> & variants, const string & VCF_1KG_file, const string & chrom );

// Parse1KGFreqs: Parse one or more VCF files from 1000 Genomes, and report
//     the frequency of each variant that appears.  The output is a map of
//     variant tag (format: <chrom>_<pos>_<ref-base>_<alt-base>) to frequency.
map<string,double>
Parse1KGFreqs( const vector<string> & VCFs_1KG );




#endif
