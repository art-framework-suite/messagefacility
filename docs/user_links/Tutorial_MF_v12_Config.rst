Tutorial for MessageFacility v1.2 Config
========================================

1. Introduction
---------------


Since MessageFacility v1.1 there has been major changes to the semantics and model of MessageFacility configuration. 
The following tutorial is to confront the latest updates in the MessageFacility configuring semantics.

The tutorial for MessageFacility v1.2 again comprises two parts, the first is the grammar and syntax for the config language (we made minor changes to it since v1.1). 
And the second part is the configuration for the MessageFacility v1.2, including several examples.

2. The Configuration Languages
------------------------------

The configuration language defines a series of "name" and "value" pairs which will be processed and parsed into some data structures that can be recognized and reused later by the code. 
Therefore, the basic component of the configuration language, or a configuration file, is assignment statements:::

    name : value

There are four types of values allowed in the configuration language,  (STYLE BUG)

#. *Primitive types*, can be an integer, a boolean, a float, a string, or a nil value 
    * nil value: :blue:`nil`
    * valid integers: :blue:`5, 0120`, etc
    * valid booleans: :blue:`true`, and :blue:`false`
    * valid float numbers: :blue:`0.5, 2.0, 120., 012e021`, etc
    * strings must be double quoted: "hello world!"(STRIKETHROUGH)
    * strings can be double quoted: :gren:`"hello world!"`
    * strings can be single quoted: :green:`'hello world!'`
    * strings can even be unquoted as long as it starts with an alphabetic and contains no spaces or breaks: :green:`hello`
#. *Vectors* of any types 
    * :blue:`[value1, value2, value3, ..., value n]`
    * vector elements can either be a primitive value, a vector, or a ParameterSet
    * It is allow to have heterogeneous types of values in a vector

#. a *ParameterSet*, which is a combination of any name value pairs
    * { name1 : value1 name2 : value2 ... name_n : value_n }
    * a ParameterSet value starts with a :blue:`"{"` and ends with a :blue:`"}"`
    * name value pairs inside of a ParameterSet must be separated by comas ","(STRIKETHROUGH)
    * name value pairs inside of a ParameterSet are separated by spaces
    * It is allowed to define a ParameterSet inside another ParameterSet, to form a nesting structure
#. *References* are a special type of values that allows you to reference the value of already defined name value pairs
    * :yellow:`name@local`
    * if the name has already been defined and completed in the same file, this reference will be replaced by the value associated with the name


Following are some examples of valid assignments:::

    a : 12
    b : 2.0
    c : true
    d : nil
    e : "tree" 
    f : []
    g : [1, 2, 3]
    h : ["str1", "str2", "str3"]
    
    i : {
      i1 : 2
      i2 : ["hydrogen", "helium"]
    }
    
    j : {
      j1 : {
        name : "tim" 
      }
    }
    
    k : e@local
    
    l : i.i2[0]@local


3. Configuration for the MessageFacility
----------------------------------------

The MessageFacility configuration file uses the above configuration language and syntax with some predefined names and rules.

3.1 Reserved Parameter Names
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

These are reserved parameter names for MessageFacility configuration. Note that names and string values are case sensitive.


debugModules
~~~~~~~~~~~~

This is a top level parameter reserved for a vector of strings tells the service to turn on the output of debug messages in the listed modules. 
Or, one can use "*" to turn on debug for all modules.


destinations
~~~~~~~~~~~~

This is a top level parameter and is reserved for a ParameterSet object which in turn includes a list of named destinations in the form of a ParameterSet object. 
When encountering each of the named destination ParameterSet object, a new destination is created and attached to the MessageFacility. 
The ParameterSet object is then used to customize the filtering and other behaviors for that destination.

Now the configuration file will look like:::

    ...
    
    debugModules : ["*"]
    suppressInfo : []
    
    destinations : {
    
        LogToConsole : {
            ...
        }
    
        LogToFile : {
            ...
        }
    
        ...
    }
    
    ...


type
~~~~

This is a string type parameter for a :blue:`NamedDestination` indicating the type of this destination. 
Available options are :blue:`"cout", "cerr", "file", "dds", and "archive"`, 
meaning routing the messages to :yellow:`stdout`, to :yellow:`stderr`, to a :yellow:`file`, :yellow:`remote DDS message server`, 
and a special type of file ( :yellow:`archive` ) used in message server specifically, respectively.

append
~~~~~~
This is a boolean type parameter for a :blue:`NamedDestination`. 
If the output of the destination is a file, this parameter is used to specify whether to discard any current content if the log file already exists 
(set to :blue:`false`, this is the default value), or just append new messages to the end of it (set to :blue:`true`)::

    append : true

threshold
~~~~~~~~~

This is a string type parameter for a :blue:`NamedDestination`. 
The destination uses the values specified in this parameter to filter messages according to there severities. 
The value means the lowest accepting severity level that will be output to the destination. 
Therefore available choices for the parameter is :blue:`"DEBUG", "INFO", "WARNING", and "ERROR"`.::

    threshold : "DEBUG"

noLineBreaks and lineLength
~~~~~~~~~~~~~~~~~~~~~~~~~~~

:green:`noLineBreaks` is a boolean type parameter for a :blue:`NamedDestination`. 
It tells the destination to turn :yellow:`on` (set to :yellow:`false`, this is the default value), 
or turn :yellow:`off` (set to :yellow:`true`) the break of lines 
when the number of characters in a single line reaches the limit specified in the int type parameter :green:`lineLength` (default value is *80* characters each line).::

    noLineBreaks : false
      lineLength : 80

noTimeStamps and useMilliseconds
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:green:`noTimeStamps` is a boolean type parameter for a :blue:`NamedDestination` to :yellow:`suppress` (when set to :yellow:`true`), 
or :yellow:`enable` (set to :yellow:`false`, this is the default value) the output of timestamps in logged messages for the destination.

:green:`useMillisecondds` is a boolean type parameter for a :blue:`NamedDestination` to switch :yellow:`on` (when set to :yellow:`true`), 
or :yellow:`off` (set to :yellow:`false`, this is the default value) the output of milliseconds in the timestamps. 
This is applicable only when :green:`noTimeStamps` is set to :yellow:`false`.::

    noTimeStamps : false
      useMilliseconds : true

outputStatistics and resetStatistics
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:green:`outputStatistics` is a boolean type parameter for a :blue:`NamedDestination`. 
If the parameter has been set to :yellow:`true`, then each time when :blue:`LogStatistics()` is being called 
the message statistic information will be append/insert to the output of this destination. Default value for the parameter has been set to :yellow:`false`.

:green:`resetStatistics` is a boolean type parameter for a :blue:`NamedDestination`. 
It tells the logger to reset (set to :yellow:`true`) the statistic information each time when :blue:`LogStatistics()` is being called, 
or not (set to :yellow:`false`, this is the default value). Applicable only when :green:`outputStatistics` has been set to :yellow:`true`.::

    outputStatistics : true
      resetStatistics : false


categories
~~~~~~~~~~

This is a ParameterSet object for a :blue:`NamedDestination`. 
It contains a list of predefined message categories (in the form of a ParameterSet object) that might appear during the job run. 
And each of the named category ParameterSet object provides the customization of message throttling and filtering to messages under this specific category, in the given destination.

The :green:`categories` ParameterSet object may have one or more ParameterSet objects with their names indicating the category name. 
A special name default is reserved for a ParameterSet object that provides the defaults for the filtering of messages whose categories are not listed in the categories object for this destination.

Note the :green:`categories` ParameterSet object only customized the behavior of messages that are logged specifically to this destination. 
Messages that are routed to other destinations will not be affected.


NamedCategory, and default
~~~~~~~~~~~~~~~~~~~~~~~~~~

These are ParameterSet objects for the :green:`categories`. 
Each of the :green:`NamedCategory` customize the message throttling for this category to this destination. 
While the :green:`default` provides the defaults for all unspecified categories.

So far, an example of the :green:`categories` parameter will look like this:::

    categories : 
    {
        unimportant : { ... }
        serious_matter : { ... }
        default : { ... }
    }


limit
~~~~~

This is an integer type parameter for a :green:`NamedCategory` or for the :green:`default` in :green:`categories`. 
It is used to instruct the logger to ignore messages after some number (the limit) have been encountered. 
(Actually, beyond the applicable limit, an occasional further message will appear, based on an exponential backoff. 
Thus, if the a limit is set at 5, and 100 messages of that id are issued, then that destination will react to the messages number 1, 2, 3, 4, 5, 10, 15, 25, 45, and 85.) 
A limit of zero will disable reporting any messages. An example,::

    limit : 20


timespan
~~~~~~~~

This is an integer type parameter for a :green:`NamedCategory` or for the default in categories. 
When a :green:`limit` is set, one can also specify that 
if no occurrences for that particular category of messages have been seen in a period of time (the :green:`timespan`), 
then the count toward that limit is to be reset. 
For an example, if one wish to suppress most of the thousands of warnings of some category expected at startup, 
but would like to know if another one happens after a gap of ten minutes, this can be specified as,::

    timespan : 600
    Edit this section
    reportEvery
    reportEvery : 20


3.2 What Does The Configuration File Look Like
----------------------------------------------

Now if we put together those reserved parameters into a configuration file, and following is how will it look like,::

    debugModules : ["*"]
        suppressInfo : []
    
        destinations : {
    
            LogToConsole : {
    
                type : "cout" 
                threshold : "INFO" 
    
                categories : {
                    unimportant : { limit : 0 }
                    serious_matter : { limit : 200 timespan : 60 }
                    default : { limit : 50 }
                }
            }
    
            LogToFile : {
    
                type : "file" 
                filename : "mylog.log" 
                append : true
    
                threshold : "DEBUG" 
    
                noTimeStamps : false
                useMilliseconds : true
    
                outputStatistics : true
                resetStatistics : false
    
                categories : {
                    unimportant : { limit : 100 }
                    serious_matter : { limit : 1000  timespan : 60 }
                    default : { limit : 200 }
                }
            }
        }



3.3 Examples
------------

* Logging messages to stdout, local file named "mylog" (overwrite the existing file), and the remote message server, with default severity threshold of INFO::

    destinations : {   
          Console : { type : "cout" }
          File    : { type : "file"  filename : "mylog" }
          Server  : { type : "dds"  }
     }

* The following example .cfg file sets up some destinations, and specifies some thresholds for how "severe" a message must be in order for it to appear in each destination.::

    destinations : { 
    
        critical : {
            type : "file" 
            filename : "critical" 
            append : true
            threshold : "ERROR" 
        }
    
        detailedInfo : {
            type : "file" 
            filename : "detailedInfo" 
            append : false
            threshold : "INFO" 
        }
    
        cerr : {
            type : "cerr" 
            threshold : "WARNING" 
        }
    }

* The following is a .cfg file appropriate for a job that will run code instrumented with many LogDebug messages. 
This hypothetical user cares only about those LogDebug messages in the category interestingToMe and, in this file, 
prefers not to see any other LogDebug or LogInfo messages.::

    debugModules : [ "*" ]
    
    destinations : {
        debugmessage : {
            threshold : "DEBUG" 
            categories : {
                interestingToMe : { limit : 1000000 }
                default : { limit : 0 }
            }
        }
    }


* The following is an example .cfg file, setting up some destinations, 
and specifying some choices for thresholds, 
and for limits and timespans applicable to severities, 
to specific message categories, and to all non-specified (default) message categroies'.



The .cfg file contains not only a list of files, but also a list of message categories that the author wishes to control individually.::

    conf_1 : {
        limit : 10
        timespan : 60
    }
    
    conf_2 : {
        limit : 20
        timespan : 60
    }
    
    conf_3 : {
        limit : 100
        timespan : 60
    }
    
    debugModules : ["*"]
    
    destinations : {
        critical : {
            type : "file" 
            filename : "critical.txt" 
    
            threshold : "ERROR" 
    
            categories : {
                default : {
                    limit : 10
                    timespan : 180
                }
                serious_matter : { limit : 100000 }
            }
        }
    
        detailedInfo : {
            type : "file" 
            filename : "deatiledInfo.txt" 
    
            threshold : "INFO" 
    
            categories : {
                default : conf_1@local
                trkwarning : conf_2@local
                unimportant : { limit : 5 }
                serious_matter : { limit : 1000000 }
            }
        }
    
        cerr : {
            type : "cerr" 
            threshold : "WARNING" 
        }
    }



