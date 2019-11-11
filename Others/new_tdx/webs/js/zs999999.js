var g_CurSel1;
var g_CurSel2;
var urlgototype = {'def':1};
if(typeof(g_zsf11)=="undefined") urlgototype['f11'] = 0;
else urlgototype['f11'] = g_zsf11;
if(typeof(g_flzx)=="undefined") urlgototype['flzx'] = 0;
else urlgototype['flzx'] = g_flzx;
if(typeof(g_bigdata)=="undefined") urlgototype['bigdata'] = 0;
else urlgototype['bigdata'] = g_bigdata;

function MouseIn(){if( Version != undefined && Version >= 2){CheckScoll();};document.getElementById("bodyid").onmouseover=null;}
function CheckScoll(){if(document.body.scrollHeight > document.body.clientHeight){document.getElementById("tsinfo").style.display="block";setTimeout( function(){document.getElementById("tsinfo").style.display="none"},2500);}}
function Exit(){window.location.reload("http://www.treeid/HIDERIGHT");}
function GotoURL(URL,type){if(urlgototype[type]){window.location.reload(URL);}}
function GotoMRZYRW(URL){window.open(URL);}
function GoToTouPiao(){window.location.reload("http://www.treeid/dlghttp://page.tdx.com.cn:7615/site/user/vote_index.htm?m=tdx_netcard&k=999999&tdxmyietitle=大盘涨跌投票&tdxmyiewidth=650&tdxmyieheight=400")}
function GoToZTTZ(name,title){window.location.reload("http://www.treeid/dlghttp://page.tdx.com.cn:7615/site/f11/zs_bkhy.html?name="+name+"&flag=0&ispc=1&tdxmyietitle="+title+"&tdxmyiewidth=1020&tdxmyieheight=700");}
function FunSCSJ(self){
    if(g_CurSel2!=self){
      g_CurSel2.className = "Nav Right";
      g_CurSel2 = self;
      g_CurSel2.className = "Nav Right Selected";
      if(self.innerText.indexOf('市场数据')!=-1) {document.getElementById("SCSJ_DIV").style.display="block";document.getElementById("SJHY_DIV").style.display="none";document.getElementById("TZBZ_DIV").style.display="none";SetCookie('SelectedLabel2','0',24*60*60*7);}
      else if(self.innerText.indexOf('事件会议')!=-1){document.getElementById("SCSJ_DIV").style.display="none";document.getElementById("SJHY_DIV").style.display="block";document.getElementById("TZBZ_DIV").style.display="none";SetCookie('SelectedLabel2','1',24*60*60*7);}
      else{document.getElementById("SCSJ_DIV").style.display="none";document.getElementById("SJHY_DIV").style.display="none";document.getElementById("TZBZ_DIV").style.display="block";SetCookie('SelectedLabel2','2',24*60*60*7);}
    }
  }
function FunPMCP(self,index){
	if(g_CurSel1!=self){
	  g_CurSel1.className = "Nav Right";
	  g_CurSel1 = self;
	  g_CurSel1.className = "Nav Right Selected";
	  if(index == 0) {document.getElementById("CPYC_DIV").style.display="block";document.getElementById("PMZJ_DIV").style.display="none";SetCookie('SelectedLabel1','0',24*60*60*7);}
	  else if(index == 1){document.getElementById("PMZJ_DIV").style.display="block";document.getElementById("CPYC_DIV").style.display="none";SetCookie('SelectedLabel1','1',24*60*60*7);}
	}
}

$(function() {
	$("#PMZJ_DIV").css('height', $("#CPYC_DIV").css('height') ); 
	$("#PMZJ_DIV .Item").each(function(){
	  var RightHeight = $(this).children("div.Right").css('height');
	  $(this).children("div.Left").css('height', (parseInt(RightHeight)+4)+'px' ); 
	  $(this).css('height', (parseInt(RightHeight)+4)+'px' ); 
	});

	document.body.oncontextmenu=document.body.ondragstart= document.body.onselectstart=document.body.onbeforecopy=function(){return false;};
	document.body.scrollTop = 0;
    document.body.onmousedown = function(e){
		e = e || window.event;
		var Y = e.clientY;
		var B = document.body.scrollTop;
		this.setCapture && this.setCapture();
		document.body.onmousemove = function(e){
			e = e || window.event;
			var diff = B + Y - e.clientY;
			document.body.scrollTop = diff;
		}
		document.body.onmouseup = function(e){
		document.body.onmousemove = null;
			this.releaseCapture && this.releaseCapture();
		}
    }

    document.getElementById("PMZJ_DIV").onmousedown = function(e){
		var evt=(e)?e:window.event;
		if (window.event) evt.cancelBubble=true; 
		else evt.stopPropagation(); 

		e = e || window.event;
		var Y = e.clientY;
		var B = document.getElementById("PMZJ_DIV").scrollTop;
		this.setCapture && this.setCapture();
		document.getElementById("PMZJ_DIV").onmousemove = function(e){
			e = e || window.event;
			var diff = B + Y - e.clientY;
			document.getElementById("PMZJ_DIV").scrollTop = diff;
		}
		document.getElementById("PMZJ_DIV").onmouseup = function(e){
			document.getElementById("PMZJ_DIV").onmousemove = null;
			this.releaseCapture && this.releaseCapture();
		}
    }
  	
  	vsgdata();

	document.getElementById("SCSJ_DIV").style.display="none";document.getElementById("SJHY_DIV").style.display="none";document.getElementById("TZBZ_DIV").style.display="none";document.getElementById("SCSJ").className="Nav Right";document.getElementById("SJHY").className="Nav Right";document.getElementById("TZBZ").className="Nav Right";
	var Sel=GetCookie('SelectedLabel2');
	if(Sel==='2'){document.getElementById("TZBZ_DIV").style.display="block";g_CurSel2=document.getElementById("TZBZ");}
	else if(Sel==='1'){document.getElementById("SJHY_DIV").style.display="block";g_CurSel2=document.getElementById("SJHY");}
	else if(Sel==='0'){document.getElementById("SCSJ_DIV").style.display="block";g_CurSel2=document.getElementById("SCSJ");}
	else{document.getElementById("SCSJ_DIV").style.display="block";g_CurSel2=document.getElementById("SCSJ");}
	g_CurSel2.className="Nav Right Selected";

	document.getElementById("CPYC_DIV").style.display="none";document.getElementById("PMZJ_DIV").style.display="none";document.getElementById("PMZJ").className="Nav Right";document.getElementById("CPYC").className="Nav Right";
	var Sel=GetCookie('SelectedLabel1');
	if(Sel==='1'){document.getElementById("PMZJ_DIV").style.display="block";g_CurSel1=document.getElementById("PMZJ");}
	else if(Sel==='0'){document.getElementById("CPYC_DIV").style.display="block";g_CurSel1=document.getElementById("CPYC");}
	else{document.getElementById("CPYC_DIV").style.display="block";g_CurSel1=document.getElementById("CPYC");}
	g_CurSel1.className="Nav Right Selected";
});