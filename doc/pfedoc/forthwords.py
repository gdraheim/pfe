from match import Match

def _unpack(text):
    quotedstring = Match("\"([.*])\"")
    if text & quotedstring:
        return quotedstring[1] & Match(r"\\(.)") >> "\\1"
    return text

usewords = {
    "P4_FXCO" : "primitive code",           "P4_FXco" : "primitive code",
    "P4_IXCO" : "immediate code",           "P4_IXco" : "immediate code",
    "P4_SXCO" : "compiling word",           "P4_SXco" : "compiling word",
    "P4_XXCO" : "load-time word",           "P4_XXco" : "load-time word",
    "P4_RTCO" : "special runtime code",     "P4_RTco" : "special runtime code",
    "P4_OVAR" : "ordinary variable",        "P4_OVaR" : "ordinary variable",
    "P4_IVAR" : "immediate variable",       "P4_IVaR" : "immediate variable",
    "P4_OCON" : "ordinary constant",        "P4_OCoN" : "ordinary constant",
    "P4_ICON" : "immediate constant",       "P4_ICoN" : "immediate constant",
    "P4_OVAL" : "ordinary value",           "P4_OVaL" : "ordinary value",
    "P4_IVAL" : "immediate value",          "P4_IVaL" : "immediate value",
    "P4_DVAR" : "dictionary variable",      "P4_DVaR" : "dictionary variable",
    "P4_DCON" : "dictionary constant",      "P4_DCoN" : "dictionary constant",
    "P4_OFFS" : "additive offset word",     "P4_OFFs" : "additive offset word",
    "P4_SNYM" : "immediate synonym",             "CO" : "primitive code",
    "P4_FNYM" : "ordinary synonym",              "CI" : "immediate code",
    "P4_iOLD" : "immediate obsolete synonym",    "CS" : "compiling word",
    "P4_xOLD" : "ordinary obsolete synonym",     "CX" : "load-time word",
    "P4_OVOC" : "ordinary vocubulary",           "OV" : "ordinary variable",
    "P4_IVOC" : "immediate vocabulary",          "OC" : "ordinary constant",
    }
ignores = {
    "P4_SLOT" : "open module-slot",
    "P4_SSIZ" : "init module-slot",
    "P4_NEED" : "issues a NEEDS check",
    "P4_INTO" : "set DEFINITIONS wordlist",
    "P4_LOAD" : "chainload a wordset",
    "P4_EXPT" : "exception id description",
    "P4_STKi" : "stackhelp info",
    "P4_STKx" : "stackhelp check",
    }
    
class ForthWord:
    def __init__(self, exports = None):
        self.exports = exports # parent
        self.typedescriptor = None
        self.has = None
        self.name = None
        self.code = None
        self.value = None
        self.oldname = None
    def parse(self, exports = None):
        if exports is not None:
            self.exports = None
        exports = self.exports
        if not exports:
            return False
        typeco = exports.typeco
        if typeco in ignores:
            return False
        if typeco not in usewords:
            print ("unknown typeco '"+typeco+"' : "+exports.export+
                   " ("+exports.get_filename()+")")
            return False
        self.typedescriptor = usewords[typeco]
        T = self.typedescriptor
        if T.endswith("code"):
            return self._parse_string_and_code()
        elif T.startswith("compiling"):
            return self._parse_string_and_code()
        elif T.startswith("load-time"):
            return self._parse_string_and_code()
        elif T.startswith("load-time"):
            return self._parse_string_and_code()
        elif T.startswith("dictionary"):
            return self._parse_string_and_code()
        elif T.startswith("dictionary"):
            return self._parse_string_and_code()
        elif T.endswith("vocabulary"):
            return self._parse_string()
        elif T.endswith("variable"):
            return self._parse_string()
        elif T.endswith("constant"):
            return self._parse_string_and_value()
        elif T.endswith("value"):
            return self._parse_string_and_value()
        elif T.endswith("offset word"):
            return self._parse_string_and_value()
        elif T.endswith("synonym"):
            return self._parse_string_and_string()
    def _parse_string(self):
        text = self.exports.export
        cuts = Match(r"^\s*(\".*\")")
        if not text & cuts:
            print ("unparseable "+self.exports.typeco+": "+text)
            return False
        self.name = _unpack(cuts[1])
        self.has = None
        self.code = None
        self.value = None
        self.oldname = None
        return True
    def _parse_string_and_code(self):
        text = self.exports.export
        cuts = Match(r"^\s*(\".*\")\s*,\s*(\w+\.*\w*)")
        if not text & cuts:
            print ("unparseable "+self.exports.typeco+": "+text)
            return False
        self.name = _unpack(cuts[1])
        self.has = "code"
        self.code = cuts[2]
        self.value = None
        self.oldname = None
        return True
    def _parse_string_and_value(self):
        text = self.exports.export
        cuts = Match(r"^\s*(\".*\")\s*,\s*(\S+)")
        if not text & cuts:
            print ("unparseable "+self.exports.typeco+": "+text)
            return False
        self.name = _unpack(cuts[1])
        self.has = "value"
        self.code = None
        self.value = cuts[2]
        self.oldname = None
        return True
    def _parse_string_and_string(self):
        text = self.exports.export
        cuts = Match(r"^\s*(\".*\")\s*,\s*(\".*\")")
        if not text & cuts:
            print ("unparseable "+self.exports.typeco+": "+text)
            return False
        self.name = _unpack(cuts[1])
        self.has = "oldname"
        self.code = None
        self.value = None
        self.oldname = _unpack(cuts[2])
        return True
    def get_typedescriptor(self):
        if not self.typedescriptor:
            if not self.parse(): return None
        return self.typedescriptor
    def get_name(self):
        if not self.name:
            if not self.parse(): return None
        return self.name
    def get_code(self):
        if not self.name:
            if not self.parse(): return None
        return self.code
    def get_value(self):
        if not self.name:
            if not self.parse(): return None
        return self.value
    def get_param(self):
        if not self.name:
            if not self.parse(): return ""
        if not self.has:
            return ""
        if self.has == "code":
            return "["+self.code+"]"
        if self.has == "value":
            return "("+self.value+")"
        if self.has == "oldname":
            return "'"+self.oldname+"'"
