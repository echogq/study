/*本js文件依赖jquery*/

/*此文件用来获取网站URL和路径*/
// var client;
var WEBSITE = {
    protocol: "",//协议，如http:/https:/file:
    host: "",    //IP或域名，如192.168.1.250、www.tdx.com.cn
    baseurl: "", //完整的网站根地址，结尾带斜杠，如http://192.168.1.250/、http://finance.tdx.com.cn/
    path: "/zdweb/test/xueqiu/",    //网站相对路径，结尾带斜杠，如/zdweb/release/，这个必须手动配置
    url: "",     //网站完整路径，结尾带斜杠，如http://192.168.1.250/zdweb/release/、http://finance.tdx.com.cn/zdweb/release/
    filepath: "",//当前访问的文件路径，如：/zdweb/release/test.html
    fileurl: "", //当前访问的完整的文件路径，如http://192.168.1.250/zdweb/release/test.html
    filename: "",//当前访问的文件名，如test.html
    filedir: "", //当前访问的文件目录,如http://192.168.1.250/zdweb/release/
    init: function () {
        this.protocol = window.location.protocol.toLowerCase();
        this.host = window.location.host.toLowerCase();
        this.baseurl = this.protocol + "//" + this.host + "/";
        this.url = this.baseurl + this.path.substr(1);
        this.filepath = window.location.pathname.replace(/\\/g, "/").toLowerCase();
        this.filename = this.filepath.replace(/(.+)[\\/]/, "");
        this.fileurl = this.baseurl + this.filepath.substr(1);
        this.filedir = this.fileurl.substring(0, this.fileurl.lastIndexOf("/") + 1);
        // client = CreateTDXClient();
        // client.connect();

        // CallTQL 兼容处理

    }
}


WEBSITE.init();


var _callFuncDict = {}; // 回调函数字典
var platform = "pc"; // pc/bs/and/ios

//判断BS平台
function IsWebPlatform() {
    return platform == "bs";
}
//判断PC平台
function IsPcPlatform() {
    return platform == "pc";
}
//判断Android平台
function IsAndPlatform() {
    return platform == "and";
}
//判断IOS平台
function IsIosPlatform() {
    return platform == "ios";
}

/*CALLTQL 默认配置*/
var CALLTQL_CONFIG = {
    url: WEBSITE.baseurl, // ajax:http://192.168.4.63/tdx/Exec",  tp_ajax:http://192.168.4.111:7615/TQLEX
    dataType: "text",
    type: "POST",
    callType: "external",//默认pc内嵌,ajax:纯网页调用,tp_ajax:tp ajax 调用,ios_external:ios内嵌调用,and_external:安卓内嵌调用,zxdialog:pc本地弹出资讯对话框
    timeout: 20000,
    CilentSpecifiedKey: ""
}

// 获取URL参数
function URLParam(name) {
    var reg = new RegExp("(^|&)" + name + "=([^&]*)(&|$)", "i");
    var sch = unescape(window.location.search);
    var r = sch.substr(1).match(reg);
    if (r != null) {
        return unescape(r[2]);
    }
    return null;
}

// 解析url页面传参
function ParseParmStr(parmStr) {
    var parms = new Object();
    strs = parmStr.split("&");
    for (var i = 0; i < strs.length; i++) {
        parms[strs[i].split("=")[0]] = strs[i].split("=")[1];
    }
    return parms;
}


// 智能切换通信层
function Auto_Get_Platform() {
    platform = "pc";
    if (window.location.href.indexOf("http") == 0) {
        platform = "bs";
    }

    // if(window.external && window.external.CallTQL){
    // 	platform="pc";
    // }

    var parmStr = decodeURI(window.location.search.substr(1));
    //console.log(parmStr)
    var paras = ParseParmStr(parmStr);
    var Ispc = paras['ispc'];
    if (Ispc == '1') {
        platform = "pc";
    }
    var ua = navigator.userAgent.toLowerCase();
    if (ua.indexOf("ipod") != -1 || ua.indexOf("iphone") != -1 || ua.indexOf("ipad") != -1) {
        platform = "ios";
    }
    else if (ua.indexOf("android") != -1) {
        platform = "and";
    }

}

function Auto_Set_Req() {
    // alert(IsPcPlatform());
    // alert(window.location.href)
    if (IsWebPlatform() || window.location.href.indexOf("http") == 0) {
        CALLTQL_CONFIG.callType = "ajax";
        if (window.location.href.indexOf("/site/") > 0)	// tp 路径带site
            CALLTQL_CONFIG.callType = "tp_ajax";
        CALLTQL_CONFIG.url = window.location.protocol + "//" + window.location.host + "/";
    }
    else if (IsPcPlatform()) {
        CALLTQL_CONFIG.callType = "external";
    }
    else if (IsIosPlatform()) {
        CALLTQL_CONFIG.callType = "ios_external";
    }
    else if (IsAndPlatform()) {
        CALLTQL_CONFIG.callType = "and_external";
    }
}

Auto_Get_Platform();
Auto_Set_Req();

/*
 * CALLTQL数据调用
 * 参数说明：
 * callback： 回调函数，不支持匿名函数的写法，如：
 *      CallTQL(function(data){
 *              //...
 *      },100101);
 * funid： 功能号
 * params： 请求的参数列表，格式：["a","b",1,2, ...]
 * config: 请求的配置，完整的配置为
 * {
 *      url:"",         //请求的网络地址
 *      dataType:"",    //返回的数据格式 支持：text/html/json/script/jsonp/... 其中script/jsonp是可跨域的
 *      type:"",        //请求的类型：GET/POST
 *      callType:"",    //js调用数据的方法 external/ajax/ios_external/and_external/zxdialog...
 *		CilentSpecifiedKey:"", // 客户端指定关键字，请求和应答，此选项保持不变
 timeout:20000,	// 超时间隔
 *		reqtype:""		// 请求类型，如果为cwserv，则是资讯原始请求,兼容以前的存储过程数组的方式,需要请求功能号，否则为默认的数据请求
 * }
 * config说明：
 * 如果不传config或者为空为NULL，
 * 则使用默认的配置，如果传了，则覆盖默认配置
 * 其中url/dataType/type/timeout只在WEB平台有效
 */
/*
 行情请求调用如下：
 沪深AB股:注：reqtype可以不用配置
 CallTQL(ret_hq,"HQServ.HQInfo",'{"Code" : "600000", "Setcode" : 1, "HasHQInfo" : 1}');
 CallTQL(ret_hq,"HQServ.ZST", '{"Code" : "600000", "Setcode" : 1, "Date" : 20140304, "HasAttachInfo" : 0}')
 CallTQL(ret_hq,"HQServ.FXT", '{"Code" : "600000", "Setcode" : 1, "Period" : 4, "Startxh" : 0, "WantNum" : 2, "TQFlag" : 0, "HasAttachInfo" : 0}')
 CallTQL(ret_hq,"HQServ.Tick", '{"Code" : "600000", "Setcode" : 1, "Date" : 20140303, "Startxh" : 0, "WantNum" : 10, "HasAttachInfo" : 1}')
 CallTQL(ret_hq,"HQServ.ZHSort", '{"SetDomain" : 14, "Period" : 0, "TopN" : 3}')
 CallTQL(ret_hq,"HQServ.CombHQ", '{"WantCol" : ["CLOSE", "MAX", "NOW"], "Secu" : [{"Code" : "600000", "Setcode" : 1},{"Code" : "600004", "Setcode" : 1}]}')
 CallTQL(ret_hq,"HQServ.MultiHQ", '{"WantCol" : ["CLOSE", "MAX", "NOW"], "SetDomain" : 14, "ColType": 6, "Startxh" : 0, "WantNum" : 28, "SortType" : 1}')
 CallTQL(ret_hq,"HQServ.SymbolFinder", '{"CodePrefix" : "60", "Offset" : 0,"MaxCount" : 10, "HaveAB" : 1, "HaveHK" : 0, "HaveQH" : 0}')
 更详细的参数请参考<<HQServ协议说明>>文档
 如果调用三方行情，需要加字段参数 "ExHQFlag":1,同时Setcode送具体的三方市场代码(参照SetDomin)) ,如：
 CallTQL(ret_hq,"HQServ.HQInfo",'{"Code" : "00001", "ExHQFlag":1,"Setcode" : 31, "HasHQInfo" : 1}',{reqtype:"raw"});
 */
function CallTQL(callback, funid, params, config) {
    config = config == undefined ? {} : config;
    var thisConfig = $.extend(true, {}, CALLTQL_CONFIG);
    //console.log(thisConfig)
    //如果有自定义配置，则覆盖默认配置
    if (config != undefined && config != "" && config != null) {
        $.each(config, function (key, value) {
            thisConfig[key] = value;
        });
    }

    var callback_name = Fun_Name(callback);
    var funcname = "CWServ.SecuInfo";
    if (typeof(params) == 'string') {	//if(thisConfig.hasOwnProperty("reqtype") && thisConfig.reqtype=="raw"){
        funcname = funid;
    }
    else {
        if (thisConfig.hasOwnProperty("reqtype") && thisConfig.reqtype == "cwserv") {
            funcname = "CWServ." + funid;
            params = FormatParams(params);
            params = "{" + params;
            if (thisConfig.CilentSpecifiedKey)
                params += ",\"CilentSpecifiedKey\":\"" + thisConfig.CilentSpecifiedKey + "\"";
            if (thisConfig.secuparse == "true")
                params += ",\"secuparse\":\"true\"";
            params += "}";


        }
        else {
            params = FormatParams(params);
            params = "{\"CallName\":\"" + funid + "\"," + params;
            if (thisConfig.CilentSpecifiedKey)
                params += ",\"CilentSpecifiedKey\":\"" + thisConfig.CilentSpecifiedKey + "\"";
            if (thisConfig.secuparse == "true")
                params += ",\"secuparse\":\"true\"";
            params += "}";

        }
    }

    if (thisConfig.callType == "ajax")  //传统网页调用
    {

        $.ajax({
            //url: thisConfig.url + "?" + getRnd(),

            url: thisConfig.url + "/tdx/Exec?" + getRnd(),
            type: thisConfig.type,
            dataType: thisConfig.dataType,
            data: {funcid: funcname, bodystr: params, timeout: thisConfig.timeout},
            jsonp: "callback",
            timeout: thisConfig.timeout,
            success: function (data, textStatus, XMLHttpRequest) {
                if (textStatus == "success") {
                    if (data.indexOf('success|') === 0) {
                        data = data.substr(8);
                        callback(data);
                    }
                    else {
                        alert("请求发生错误！" + data);
                    }
                }
            },
            error: function (xhr, stat, err) {
                if (stat == "timeout") {
                    callback('{"ErrorCode":1, "ErrorInfo":"timeout"}')
                }
            }
        });
    }
    else if (thisConfig.callType == "tp_ajax")  // tp后台网页调用
    {
        var callFunc = (config || {})["callFunc"];
        if (callFunc) {
            funcname = callFunc;
        }
        $.ajax({
            url: thisConfig.url + 'TQLEX?Entry=' + funcname,
            type: thisConfig.type,
            dataType: thisConfig.dataType,
            data: params,
            jsonp: "callback",
            timeout: thisConfig.timeout,
            success: function (data) {
                callback(data);
            },
            error: function (xhr, stat, err) {
                if (stat == "timeout") {
                    callback('{"ErrorCode":1, "ErrorInfo":"timeout"}')
                }
            }
        });
        // client.CallTQL(funcname, params, function(err, res){
        // 	if (err)
        // 	{

        // 	}
        // 	else
        // 	{
        // 		callback(data);
        // 	}
        // });
    }
    else if (thisConfig.callType == "external") //pc 接口调用
    {
        try {
            window.external.CallTQL(callback_name, funcname, params);
        }
        catch (e) {
            alert("此JS平台不支持window.external.CallTQL方法。\r\n\r\n错误提示：" + e.message);
        }
    }
    else if (thisConfig.callType == "and_external") //andriod 接口调用
    {
        var formid = getRnd() + '_' + callback_name;
        _callFuncDict[formid] = callback;
        var session = "HQSession";
        if (config && config["SendSession"] && config["SendSession"].length > 0) {
            session = config["SendSession"];
        }

        // window.tdx_java.Android_SendData(formid, funcname, 'TP', params.length, params, "Get_Android");
        var strOptions = "{\"SendSession\":\"" + session + "\"}"
        // alert([funcname, params, strOptions,"Get_Android", formid])
        window.tdx_java.TdxSendData(funcname, params, strOptions, "Get_Android", formid)
    }
    else if (thisConfig.callType == "ios_external") //ios 接口调用
    {
        var formid = getRnd() + '_' + callback_name;
        _callFuncDict[formid] = callback;
        var session = "HQSession";
        if (config["SendSession"] && config["SendSession"].length > 0) {
            session = config["SendSession"];
        }
        params = "{\"SessionType\":\"" + session + "\"" + ",\"TQLParam\":" + params + "}";
        IOS_Bridge.call("CallTQL", params, funcname, formid, "Get_Ios");
    }
    else if (thisConfig.callType == "zxdialog") {	// pc资讯弹出对话框调用
        try {
            window.external.CallTQL_IE(Fun_Name(callback), funcname, params);
        }
        catch (e) {
            alert("此JS平台不支持window.external.CallTQL_IE方法。\r\n\r\n错误提示：" + e.message);
        }
    }
    else {
        alert("不支持的JS调用类型：" + thisConfig.callType);
    }
}

/*
 * 返回URL的一个随机串
 */
function getRnd() {
    return parseInt(Math.random() * 10000) + "=" + parseInt(Math.random() * 10000);
}

/*
 * 返回当前时间赋值F_RUNTIME
 */
function getRuntime() {
    var date = new Date();
    var now = "";
    var year = date.getFullYear();
    var month = date.getMonth() + 1;
    var date2 = date.getDate();
    var hour = date.getHours();
    var min = date.getMinutes();
    var sec = date.getSeconds();
    now = year + "-" + month + "-" + date2 + " " + hour + ":" + min + ":" + sec;
    return now;
}

/*
 * 获取 function 名称
 */
function Fun_Name(funname) {
    var tmp = funname.toString();
    var re = /function\s*(\w*)/i;
    var matches = re.exec(tmp);
    //alert("name:" +matches[1]);
    return matches[1];
}

/*FormatParams请求函数，用于格式化参数*/
function FormatParams(pa) {
    var s = "";
    s += "\"Params\":[";
    var pas = "";
    for (var i = 0; i < pa.length; i++) {
        if (typeof (pa[i]) == 'string') {
            pas += ",\"" + pa[i] + "\"";
        } else if (typeof (pa[i]) == 'number') {
            pas += "," + pa[i];
        }
        else {
            alert("非法的参数！");
            break;
        }
    }
    if (pas != "") {
        pas = pas.substr(1);
    }
    s += pas;
    s += "]";
    return s;
}

/*
 原有格式
 {
 "ErrorCode":0,
 "ErrorInfo":"",
 "ResultSetNum":2,
 "CilentSpecifiedKey":"",
 "ResultSets":[
 {
 "ColNum":2,
 "RowNum":3,
 "ColDes":[
 {"Name":"D01","Type":"VARCHAR"},
 {"Name":"D02","Type":"VARCHAR"}
 ],
 "Content":[
 ["aaa","bbb"],
 ["ccc","ddd"],
 ["eee","fff"]
 ]
 },
 {
 "ColNum":2,
 "RowNum":3,
 "ColDes":[
 {"Name":"D01","Type":"VARCHAR"},
 {"Name":"D02","Type":"VARCHAR"}
 ],
 "Content":[
 ["aaa","bbb"],
 ["ccc","ddd"],
 ["eee","fff"]
 ]
 }
 ]
 }

 JSON数据解析，解析后的格式：
 * {
 *    ErrorCode:"",
 *    ErrorInfo:"",
 *    tables:[
 *        {
 *            total:"",rows:[
 *                {"T001":"012","T002":"01201","T003":"aaa"},
 *                {"T001":"012","T002":"01202","T003":"bbb"}
 *            ]
 *        }，
 *        {
 *            total:"",rows:[
 *                {"T001":"012","T002":"01201","T003":"aaa"},
 *                {"T001":"012","T002":"01202","T003":"bbb"}
 *            ]
 *        }
 *    ]
 * }
 */
// 资讯数据规整
function FormatResult(data) {
    try {

        if (IsAndPlatform() || IsIosPlatform()) {
            data = data.replace(/\r\n/ig, "<br>");
        }
        var d = $.parseJSON(data);
    }
    catch (e) {
        //alert("错误提示："+e.message);
    }
    var rd = {};

    rd.ErrorCode = d.ErrorCode;

    if (rd.ErrorCode != 0) {
        rd.ErrorInfo = d.ErrorInfo;
        return rd;
    }
    rd.CilentSpecifiedKey = d.CilentSpecifiedKey;
    rd.tables = [];
    for (var i = 0; i < d.ResultSets.length; i++) {
        if (d.ResultSets[i].ColDes[0].Name == "total") {
            if (i - 1 >= 0) {
                rd.tables[i - 1].total = d.ResultSets[i].Content[0][0];
            }
        }
        rd.tables[i] = {};
        rd.tables[i].total = "";
        rd.tables[i].rows = [];
        var cont = d.ResultSets[i].Content;
        for (var j = 0; j < cont.length; j++) {
            rd.tables[i].rows[j] = {};
            var cols = d.ResultSets[i].ColDes;
            for (var k = 0; k < cols.length; k++) {
                rd.tables[i].rows[j][cols[k].Name] = cont[j][k];
            }
        }
    }
    return rd;
}


/****************IOS/andriod通信协议*******************/
/*
 * IOS函数桥接
 */
var IOS_Bridge = {

    callbacksCount: 1,
    callbacks: {},

    resultForCallback: function resultForCallback(callbackId, resultArray) {
        try {
            var callback = IOS_Bridge.callbacks[callbackId];
            if (!callback) {
                return;
            }
            callback.apply(null, resultArray);
        } catch (e) {
            alert(e);
        }
    },

    /*
     * functionName: 调用函数名称
     * args:json 格式参数
     * callback 回调函数，可有可无
     */
    call: function call(functionName, args, funcID, frameID, callback) {

        var hasCallback = callback && typeof callback == "function";
        var callbackId = hasCallback ? IOS_Bridge.callbacksCount++ : 0;

        if (hasCallback) {
            IOS_Bridge.callbacks[callbackId] = callback;
        }
        var iframe = document.createElement("IFRAME");
        if (functionName == "Get_Ret") {
            iframe.setAttribute("src", "js-frame:" + functionName + ";;" + callbackId + ";;" + frameID + ";;" + funcID + ";;" + args + ";;" + callback + ";;" + "");

        }
        else {
            iframe.setAttribute("src", "tdx-frame:" + functionName + ";;" + frameID + ";;" + funcID + ";;" + args + ";;" + callback);
        }


        //encodeURIComponent(JSON.stringify(args)));
        document.documentElement.appendChild(iframe);
        iframe.parentNode.removeChild(iframe);
        iframe = null;

    }
};

function Get_Ios(formid, funcid, flagtype, _data, callbackname) {
    if (_callFuncDict.hasOwnProperty(formid)) {
        var callback = _callFuncDict[formid];
        _callFuncDict[formid] = undefined;
        delete _callFuncDict[formid];
        callback(_data);
    }
    // window.frames["Main_Frame"].Cmd_cbk(formid, funcid, flagtype, _data, callbackname);
}

//统一不同方式的回调函数
function Get_Android(formid, funcid, flagtype, _data) {
    if (_callFuncDict.hasOwnProperty(formid)) {
        var callback = _callFuncDict[formid];
        _callFuncDict[formid] = undefined;
        delete _callFuncDict[formid];
        callback(_data);

    }
}
/****************the end---IOS/andriod通信协议*******************/
