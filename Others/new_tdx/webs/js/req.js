if(window.TDXQuery){
  window.jscall=function(opts,callback){
    var req=JSON.stringify({
      Method:opts.path||'SYS_FUNCData',
      FuncName:opts.entry||'',
      Param:opts.params||''
    });
    callback=callback||function(){};
    window.TDXQuery({
      request: req,
      onSuccess: function(response) {
        callback(0,response);
      },
      onFailure: function(ErrorCode, ErrorInfo) {
        callback(ErrorCode,ErrorInfo);
      }
    });
  }
}

function urlparam(name) {  
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)");  
    var r = window.location.search.substr(1).match(reg);  
    if (r != null) return unescape(r[2]); return null;  
}  