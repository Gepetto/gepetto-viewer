# Doxyfile 1.4.2


#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------
OUTPUT_DIRECTORY       = .
CREATE_SUBDIRS         = NO
OUTPUT_LANGUAGE        = English
BRIEF_MEMBER_DESC      = YES
REPEAT_BRIEF           = YES
ABBREVIATE_BRIEF       = "The $name class" \
                         "The $name widget" \
                         "The $name file" \
                         is \
                         provides \
                         specifies \
                         contains \
                         represents \
                         a \
                         an \
                         the
ALWAYS_DETAILED_SEC    = YES
INLINE_INHERITED_MEMB  = NO
FULL_PATH_NAMES        = NO
STRIP_FROM_PATH        = 
STRIP_FROM_INC_PATH    = 
SHORT_NAMES            = NO
JAVADOC_AUTOBRIEF      = YES
MULTILINE_CPP_IS_BRIEF = NO
DETAILS_AT_TOP         = YES
INHERIT_DOCS           = YES
DISTRIBUTE_GROUP_DOC   = NO
SEPARATE_MEMBER_PAGES  = NO
TAB_SIZE               = 2
OPTIMIZE_OUTPUT_FOR_C  = NO
OPTIMIZE_OUTPUT_JAVA   = NO
SUBGROUPING            = YES

#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------
EXTRACT_LOCAL_CLASSES  = YES
EXTRACT_LOCAL_METHODS  = NO
CASE_SENSE_NAMES       = NO
HIDE_SCOPE_NAMES       = NO
SHOW_INCLUDE_FILES     = NO
INLINE_INFO            = YES
SORT_MEMBER_DOCS       = YES
SORT_BRIEF_DOCS        = NO
SORT_BY_SCOPE_NAME     = NO
MAX_INITIALIZER_LINES  = 30
FILE_VERSION_FILTER    = 
#---------------------------------------------------------------------------
# configuration options related to warning and progress messages
#---------------------------------------------------------------------------
QUIET                  = NO
WARNINGS               = YES
WARN_IF_UNDOCUMENTED   = YES
WARN_IF_DOC_ERROR      = YES
WARN_NO_PARAMDOC       = NO
WARN_FORMAT            = "$file:$line: $text"
WARN_LOGFILE           = 
#---------------------------------------------------------------------------
# configuration options related to the input files
#---------------------------------------------------------------------------
RECURSIVE              = YES
EXCLUDE                = 
EXCLUDE_SYMLINKS       = NO
EXAMPLE_PATH           = 
EXAMPLE_PATTERNS       = *
EXAMPLE_RECURSIVE      = NO
INPUT_FILTER           = 
FILTER_PATTERNS        = 
FILTER_SOURCE_FILES    = NO
#---------------------------------------------------------------------------
# configuration options related to source browsing
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
# configuration options related to the alphabetical class index
#---------------------------------------------------------------------------
ALPHABETICAL_INDEX     = NO
COLS_IN_ALPHA_INDEX    = 5
IGNORE_PREFIX          = 
#---------------------------------------------------------------------------
# configuration options related to the HTML output
#---------------------------------------------------------------------------
GENERATE_HTML          = YES
HTML_FILE_EXTENSION    = .html
HTML_ALIGN_MEMBERS     = YES
GENERATE_HTMLHELP      = NO
CHM_FILE               = 
HHC_LOCATION           = 
GENERATE_CHI           = NO
BINARY_TOC             = NO
TOC_EXPAND             = YES
DISABLE_INDEX          = NO
ENUM_VALUES_PER_LINE   = 1
GENERATE_TREEVIEW      = YES
TREEVIEW_WIDTH         = 250
#---------------------------------------------------------------------------
# configuration options related to the LaTeX output
#---------------------------------------------------------------------------
LATEX_OUTPUT           = latex
LATEX_CMD_NAME         = latex
MAKEINDEX_CMD_NAME     = makeindex
COMPACT_LATEX          = NO
PAPER_TYPE             = a4wide
EXTRA_PACKAGES         = 
LATEX_HEADER           = 
PDF_HYPERLINKS         = NO
USE_PDFLATEX           = NO
LATEX_BATCHMODE        = NO
LATEX_HIDE_INDICES     = NO
#---------------------------------------------------------------------------
# configuration options related to the RTF output
#---------------------------------------------------------------------------
RTF_OUTPUT             = rtf
COMPACT_RTF            = NO
RTF_HYPERLINKS         = NO
RTF_STYLESHEET_FILE    = 
RTF_EXTENSIONS_FILE    = 
#---------------------------------------------------------------------------
# configuration options related to the man page output
#---------------------------------------------------------------------------
MAN_OUTPUT             = man
MAN_EXTENSION          = .3
MAN_LINKS              = NO
#---------------------------------------------------------------------------
# configuration options related to the XML output
#---------------------------------------------------------------------------
XML_OUTPUT             = xml
XML_SCHEMA             = 
XML_DTD                = 
XML_PROGRAMLISTING     = YES
#---------------------------------------------------------------------------
# configuration options for the AutoGen Definitions output
#---------------------------------------------------------------------------
GENERATE_AUTOGEN_DEF   = NO
#---------------------------------------------------------------------------
# configuration options related to the Perl module output
#---------------------------------------------------------------------------
PERLMOD_LATEX          = NO
PERLMOD_PRETTY         = YES
PERLMOD_MAKEVAR_PREFIX = 
#---------------------------------------------------------------------------
# Configuration options related to the preprocessor   
#---------------------------------------------------------------------------
ENABLE_PREPROCESSING   = YES
MACRO_EXPANSION        = YES
EXPAND_ONLY_PREDEF     = YES
SEARCH_INCLUDES        = YES
INCLUDE_PATH           = 
INCLUDE_FILE_PATTERNS  = 
EXPAND_AS_DEFINED      = 
SKIP_FUNCTION_MACROS   = YES
#---------------------------------------------------------------------------
# Configuration::additions related to external references   
#---------------------------------------------------------------------------
TAGFILES               = \
	${KWSPLUS_DOCDIR}/html/kwsPlus.doxytag=${KWSPLUS_DOCDIR}/html \
	${HPPMODEL_DOCDIR}/html/hppModel.doxytag=${HPPMODEL_DOCDIR}/html
GENERATE_TAGFILE       = hppCore.doxytag
ALLEXTERNALS           = NO
EXTERNAL_GROUPS        = YES
PERL_PATH              = /usr/bin/perl
#---------------------------------------------------------------------------
# Configuration options related to the dot tool   
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
# Configuration::additions related to the search engine   
#---------------------------------------------------------------------------
SEARCHENGINE           = NO




#---------------------------------------------------------------------------
# Project related configuration options
#---------------------------------------------------------------------------
PROJECT_NAME           = "${PROJECT_NAME} library documentation"
PROJECT_NUMBER         = ${PROJECT_VERSION}

#---------------------------------------------------------------------------
# Build related configuration options
#---------------------------------------------------------------------------
EXTRACT_ALL            = YES
EXTRACT_PRIVATE        = YES
EXTRACT_STATIC         = NO
HIDE_UNDOC_MEMBERS     = YES
HIDE_UNDOC_CLASSES     = YES
HIDE_FRIEND_COMPOUNDS  = YES
HIDE_IN_BODY_DOCS      = NO
INTERNAL_DOCS          = NO
SHOW_INCLUDE_FILES     = NO
GENERATE_TODOLIST      = YES
GENERATE_TESTLIST      = NO
GENERATE_BUGLIST       = NO
GENERATE_DEPRECATEDLIST= YES
ENABLED_SECTIONS       = 
SHOW_USED_FILES        = NO
SHOW_DIRECTORIES       = NO
#---------------------------------------------------------------------------
# configuration options related to warning and progress messages
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
# configuration options related to the input files
#---------------------------------------------------------------------------
FILE_PATTERNS          = *.hh *.idl
EXCLUDE_PATTERNS       = 
INPUT                  = ${${PROJECT_NAME}_SOURCE_DIR}/include \
                         ${CMAKE_CURRENT_SOURCE_DIR}/additionalDoc

#---------------------------------------------------------------------------
# configuration options related to inserting images
#---------------------------------------------------------------------------
IMAGE_PATH = ${${PROJECT_NAME}_SOURCE_DIR}/doc/figures
#---------------------------------------------------------------------------
# configuration options related to source browsing
#---------------------------------------------------------------------------
SOURCE_BROWSER         = NO
INLINE_SOURCES         = NO
STRIP_CODE_COMMENTS    = YES
REFERENCED_BY_RELATION = NO
REFERENCES_RELATION    = NO
VERBATIM_HEADERS       = NO
#---------------------------------------------------------------------------
# configuration options related to the alphabetical class index
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
# configuration options related to the HTML output
#---------------------------------------------------------------------------
HTML_OUTPUT            = html
HTML_STYLESHEET        = ${CMAKE_CURRENT_SOURCE_DIR}/package.css
HTML_HEADER            = ${CMAKE_CURRENT_SOURCE_DIR}/header.html
HTML_FOOTER            = ${CMAKE_CURRENT_SOURCE_DIR}/footer.html

#---------------------------------------------------------------------------
# configuration options related to the LaTeX output
#---------------------------------------------------------------------------
GENERATE_LATEX         = NO
USE_PDFLATEX           = YES
#---------------------------------------------------------------------------
# configuration options related to the RTF output
#---------------------------------------------------------------------------
GENERATE_RTF           = NO
#---------------------------------------------------------------------------
# configuration options related to the man page output
#---------------------------------------------------------------------------
GENERATE_MAN           = NO
#---------------------------------------------------------------------------
# configuration options related to the XML output
#---------------------------------------------------------------------------
GENERATE_XML           = NO
#---------------------------------------------------------------------------
# configuration options for the AutoGen Definitions output
#---------------------------------------------------------------------------
GENERATE_AUTOGEN_DEF   = NO
#---------------------------------------------------------------------------
# configuration options related to the Perl module output
#---------------------------------------------------------------------------
GENERATE_PERLMOD       = NO
#---------------------------------------------------------------------------
# Configuration options related to the preprocessor   
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
# Configuration::additions related to external references   
#---------------------------------------------------------------------------
#---------------------------------------------------------------------------
# Configuration options related to the dot tool   
#---------------------------------------------------------------------------
CLASS_DIAGRAMS         = YES
HIDE_UNDOC_RELATIONS   = YES
HAVE_DOT               = YES
CLASS_GRAPH            = NO
COLLABORATION_GRAPH    = YES
GROUP_GRAPHS           = YES
UML_LOOK               = NO
TEMPLATE_RELATIONS     = NO
INCLUDE_GRAPH          = YES
INCLUDED_BY_GRAPH      = YES
CALL_GRAPH             = NO
GRAPHICAL_HIERARCHY    = YES
DIRECTORY_GRAPH        = YES
DOT_IMAGE_FORMAT       = png
DOT_PATH               = 
DOTFILE_DIRS           = 
MAX_DOT_GRAPH_DEPTH    = 1000
DOT_TRANSPARENT        = NO
DOT_MULTI_TARGETS      = YES
GENERATE_LEGEND        = YES
DOT_CLEANUP            = YES
