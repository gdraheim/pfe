from match import Match

class ForthNotation:
    def __init__(self, text, header = None):
        self.header = None
        self.text = text
        self.name = None
        self.stack = None
        self.hints = None
    def get_filename(self):
        if self.header:
            return self.header.get_filename()
        return None
    def parse(self, text = None):
        if text is not None:
            self.text = text
        text = self.text
        if text is None:
            return False
        m1 = Match(r"^\s*'([^']+)'\s+(\([^\(\)]*--[^\(\)]*\))(.*)")
        m2 = Match(r"^\s*\"([^\"]+)\"\s+(\([^\(\)]*--[^\(\)]*\))(.*)")
        m3 = Match(r"^\s*([^\(\)\s]+)\s+(\([^\(\)]*--[^\(\)]*\))(.*)")
        if text & m1:
            self.name = m1[1] ; self.stack = m1[2] ; self.hints = m1[3]
            self.name &= Match(r"\\(.)") >> "\\1"
            return True
        elif text & m2:
            self.name = m2[1] ; self.stack = m2[2] ; self.hints = m2[3]
            self.name &= Match(r"\\(.)") >> "\\1"
            return True
        elif text & m3:
            self.name = m3[1] ; self.stack = m3[2] ; self.hints = m3[3]
            return True
        else:
            return False
    def _parsed(self):
        if not self.name:
            return self.parse()
        return True
    def get_name(self):
        if not self._parsed(): return None
        return self.name
    def get_stack(self):
        if not self._parsed(): return None
        return self.stack
    def get_hints(self):
        if not self._parsed(): return None
        return self.hints
