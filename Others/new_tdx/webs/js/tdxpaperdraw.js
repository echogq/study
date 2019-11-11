/*
作者: tdx_zy
说明: 依赖raphael.js自绘
用法: 必须注册才可用
*/
var Version = 3;
function getStyleEle(oElement){
    return oElement.currentStyle ? oElement.currentStyle : getComputedStyle(oElement, null);
}
g_BlackClrL = ['#2d2d2d', '#232323', '#d5d5d5', '#ff3d01', '#07f407'];
g_WhiteClrL = ['#939fad', '#d3d9de', '#000000', '#ff3d01', '#07f407'];

var yahei='0'
if(typeof(g_yahei)!=="undefined")
	yahei=g_yahei
var isIESystem=true
if (!!window.ActiveXObject || "ActiveXObject" in window)
    isIESystem=true;
else
    isIESystem= false;

var g_rapFontid = 0;
var g_rapHanZiFontName = ['黑体']
var g_rapHanZiFontSize = ['11px']
var g_rapShuZiFontName = ['黑体']
var g_rapShuZiFontSize = ['12px']
var g_raFontFamily='arial'
var g_fontSize="11px"
if (yahei=="1"){
    g_fontSize="10px"
	g_rapHanZiFontName = ['Microsoft YaHei,黑体,arial, Helvetica, sans-serif']
	g_rapShuZiFontName= ['Microsoft YaHei,黑体,arial, Helvetica, sans-serif']
    g_raFontFamily='"Microsoft YaHei", 微软雅黑, arial, Helvetica, sans-serif'
}
var tdxPaperDraw={
//注册入ToRaphael中；画布对象可以直接调用
RegisterToRaphael : function (Paper){
	Paper.tdxReportBar=tdxPaperDraw.ReportBar;
	Paper.tdxSpiderPic=tdxPaperDraw.SpiderPic;
	Paper.tdxLinePic=tdxPaperDraw.LinePic;
},

/**/
ReportBar : function (LeftW,BottomH,sUint){
	if(this instanceof Raphael){
		tdxPaperDraw.ReportBar.Paper=this;
	}
	if(!(this instanceof tdxPaperDraw.ReportBar)){
		return new tdxPaperDraw.ReportBar(LeftW,BottomH,sUint);
	}
	if(tdxPaperDraw.ReportBar.Paper){
		this.Paper=tdxPaperDraw.ReportBar.Paper; //this.Paper是该画布
	}
	this.PathSet=this.Paper.set();
	if(arguments.length<3){
		this.LeftW=30;
		this.BottomH=20;
		this.x=0;
		this.y=10;
		this.sUint='';
	}
	else{
		this.LeftW=LeftW;
		this.BottomH=BottomH;
		this.x=LeftW;
		this.y=10;
		this.sUint=sUint;
	}
	this.Tooltip;
	this.w = 160;
    if(Version>=3)
	    this.h = 75;
    else
        this.h = 95;
	this.BottomPointX=[this.x+this.w/3-this.w/6, this.x+this.w*2/3-this.w/6, this.x+this.w-this.w/6];
	this.BottomPointY=this.y+this.h;
	this.ClrList = g_BlackClrL;
	var body = document.getElementById("bodyid");
    var currenSty=getStyleEle(body)
    if(currenSty){
        var colorS=currenSty["color"]+""
        colorS=colorS.toUpperCase()
        if(colorS=='#000'||colorS=='#000000'||colorS=='RGB(0, 0, 0)'||colorS=='RGB(0,0,0)')
            this.ClrList = g_WhiteClrL;
    }

	//外框clr1 内线clr2 字clr3 bar clr4

	this.setBarAttr = function(){
		clr1 = this.ClrList[0];
		clr2 = this.ClrList[1];
		var strLine = 'M'+(this.x)+','+(this.y);
		strLine += ' L'+(this.x)+','+(this.y+this.h);
		strLine += ' L'+(this.x+this.w)+','+(this.y+this.h);

		var pNode=this.Paper.path(strLine).attr({
			'stroke-width':1,
			'stroke':clr1
		})
        if(!isIESystem){
            pNode.translate(0.5,0.5)
            pNode.node.style["shape-rendering"]="crispEdges";

        }
		/*for(var i=1;i<4;i++){
			var strLine = 'M'+(this.x)+','+(this.y+i*this.h/4)+' L'+(this.x+this.w)+','+(this.y+i*this.h/4);
			this.Paper.path(strLine).attr({
				'stroke-width':1,
				'stroke':clr2
			});
			var strLine = 'M'+(this.BottomPointX[i-1])+','+(this.y)+' L'+(this.BottomPointX[i-1])+','+(this.y+this.h);
			this.Paper.path(strLine).attr({
				'stroke-width':1,
				'stroke':clr2
			});
		}*/
	}
	
	//data: [['2012',val1,],['2011',val1,val2..]] 大到小 barcnt:索引号;如果是99表示所有
	this.setDayReportBar=function(data,barcnt,des){
		var self = this;
		if(this.PathSet){this.PathSet.remove();}
		if(data && data.length>0){
			var LDate=[];
			var LVal=[];
			for(var i = 0;i<data.length&&i<3;i++){
				LDate.push(data[i][0]);
				if(barcnt === 99){
					for(var j=1;j<data[i].length;j++){
						if(!isNaN(parseFloat(data[i][j]))){
							LVal.push(Math.abs(parseFloat(data[i][j])));
						}
					}
				}
				else{
					if(!isNaN(parseFloat(data[i][barcnt])))
						LVal.push(Math.abs(parseFloat(data[i][barcnt])));
				}
			}
			var MAX = Math.max.apply(null,LVal);
			var MIN = Math.min.apply(null,LVal);

			if(isFinite(MAX) == false) MAX = 0;
			if(isFinite(MIN) == false) MIN = 0;

			var strLen = (Math.abs(parseInt(MAX)).toString()).length;
			var PerVal = (MAX - 0)/7;
			var PerValPX;
			var LLeft;
			if(MAX==0){
				LLeft = [0,1,2,3,4];
				PerValPX = this.h/(LLeft[4]-LLeft[0]);
			}
			else{
				PerValPX = this.h/(Math.abs(PerVal)*8);
				LLeft = [0,2*PerVal,4*PerVal,6*PerVal,8*PerVal];
			}
			switch(strLen){
				case 1:
					des = des +'('+'元'+')';break;
				case 2:
					des = des +'('+'十元'+')';break;
				case 3:
					des = des +'('+'百元'+')';break;
				case 4:
					des = des +'('+'千元'+')';break;
				case 5:
					des = des +'('+'万'+')';break;
				case 6:
					des = des +'('+'十万'+')';break;
				case 7:
					des = des +'('+'百万'+')';break;
				case 8:
					des = des +'('+'千万'+')';break;
				case 9:
					des = des +'('+'亿'+')';break;
				case 10:
					des = des +'('+'十亿'+')';break;
				case 11:
					des = des +'('+'百亿'+')';break;
				case 12:
					des = des +'('+'千亿'+')';break;
				case 13:
					des = des +'('+'万亿'+')';break;
				default:
					des = des +'('+'元'+')';
			}
			//画左边
			for(var i = LLeft.length-1; i>=0; --i){
				var sText;
				if(strLen<=1)
					sText = (LLeft[i]).toFixed(2);
				else
					sText = (LLeft[i]/(Math.pow(10,strLen-1))).toFixed(2);

				sText = sText.toString();
				this.PathSet.push(this.Paper.text(this.x-2, this.y+(LLeft.length-i-1)*this.h/4, sText).attr({
					'fill':this.ClrList[2],

					'font-family': g_raFontFamily,
					'font-size':g_fontSize,
					'font-weight': 'normal',
					'text-anchor': 'end'
				}));
			}
			//画日期
			for(var i = 0;i<data.length&&i<3;i++){
				this.PathSet.push(this.Paper.text(this.BottomPointX[i], this.BottomPointY+10, LDate[i]).attr({
					'fill':this.ClrList[2],
					'font-family': g_raFontFamily,
					'font-size':g_fontSize,
					'font-weight': 'normal'
				}));
			}
			//画bar
			if(barcnt === 99){
				for(var i = 0;i<data.length&&i<3;i++){
					var BarW = 6;
					var DiffW = 2;
					var sPx = this.BottomPointX[i]-BarW*2-DiffW/2*3;
					for(var j=0; j<barcnt; j++){
						var Barval = parseFloat(data[i][j+1]);
						if(!isNaN(Barval)){
							var clr = this.ClrList[3];
							if(Barval<0) clr = this.ClrList[4];
							var BarH = Math.abs(Barval) * PerValPX;
							if(BarH<3)
								BarH = 3;
							var BarElm = this.Paper.rect(sPx+j*(BarW+DiffW), this.BottomPointY-BarH-1, BarW, BarH).attr({
								'fill':clr,
								'stroke-width':0
							})
							this.PathSet.push( BarElm );
							BarElm.tooltipval = this.Format(Barval);
							BarElm.cX = this.x+30;
							BarElm.cY = this.y+1;
							BarElm.clr = clr;
							BarElm.mouseover(function(){
								if(self.Tooltip){
									self.Tooltip.remove();
								}
								self.Tooltip = self.Paper.text(this.cX, this.cY, this.tooltipval).attr({
									'fill':this.clr,
									'font-family': g_raFontFamily,
									'font-size':g_fontSize,
									'font-weight': 'normal',
									'text-anchor': 'middle'
								});
								
							});
							BarElm.mouseout(function(){
								if(self.Tooltip){
									self.Tooltip.remove();
								}
							});
						}
					}
				}
			}
			else{
				for(var i = 0;i<data.length&&i<3;i++){
					var BarW = 14;
					var Barval = parseFloat(data[i][barcnt]);
					if(!isNaN(Barval)){
						var clr = this.ClrList[3];
						if(Barval<0) clr = this.ClrList[4];
						var BarH = Math.abs(Barval) * PerValPX;
						if(BarH<3)
							BarH = 3;
						var BarElm = this.Paper.rect(this.BottomPointX[i]-BarW/2, this.BottomPointY-BarH-1, BarW, BarH).attr({
							'fill':clr,
							'stroke-width':0
						})
						this.PathSet.push( BarElm );
						BarElm.tooltipval = this.Format(Barval);
						BarElm.cX = this.x+30;
						BarElm.cY = this.y+1;
						BarElm.clr = clr;
						BarElm.mouseover(function(){
							if(self.Tooltip){
								self.Tooltip.remove();
							}
							self.Tooltip = self.Paper.text(this.cX, this.cY, this.tooltipval).attr({
								'fill':this.clr,
								'font-family': g_raFontFamily,
								'font-size':g_fontSize,
								'font-weight': 'normal',
								'text-anchor': 'middle'
							});
							
						});
						BarElm.mouseout(function(){
							if(self.Tooltip){
								self.Tooltip.remove();
							}
						});
					}
				}
			}
			this.PathSet.push(this.Paper.text(this.x+this.w, this.y, des).attr({
				'fill':this.ClrList[2],
				'font-family': g_raFontFamily,
				'font-size':g_fontSize,
				'font-weight': 'normal',
				'text-anchor': 'end'
			}));
		}
	}

	this.Format = function(val){
		try{
			var v = parseFloat(val);
			if (v>=10000000000 || v<=-10000000000){
				v = v/100000000;
				return v.toFixed(0)+'亿';
			}
			else if(v>=100000000 || v<=-100000000){
				v = v/100000000;
				return v.toFixed(2)+'亿';
			}
			else if(v>=1000000 || v<=-1000000){
				v = v/10000;
				return v.toFixed(0)+'万';
			}
			else if(v>=10000 || v<=-10000){
				v = v/10000;
				return v.toFixed(2)+'万';
			}
			else{
				return v.toFixed(2)+'元';
			}
		}
		catch(e){
			return '--'
		}
	}
},

SpiderPic : function (){
	if(this instanceof Raphael){
		tdxPaperDraw.SpiderPic.Paper=this;
	}
	if(!(this instanceof tdxPaperDraw.SpiderPic)){
		return new tdxPaperDraw.SpiderPic();
	}
	if(tdxPaperDraw.SpiderPic.Paper){
		this.Paper=tdxPaperDraw.SpiderPic.Paper; //this.Paper是该画布
	}
	self.Tooltip;
	this.PathSet=this.Paper.set();
	this.x = 93;
	this.y = 100;
	this.Elm1;
	this.Elm2;
	this.Elm3;
	this.Elm4;
	this.Elm5;

	this.BKClr = 0;//0-黑色背景 1-白色背景
	var body = document.getElementById("bodyid");
    var currenSty=getStyleEle(body)
    if(currenSty){
        var colorS=currenSty["color"]+""
        colorS=colorS.toUpperCase()
        if(colorS=='#000'||colorS=='#000000'||colorS=='RGB(0, 0, 0)'||colorS=='RGB(0,0,0)')
            this.BKClr = 1;
    }


	
	this.L = [15,25,35,45,55];
	this.H = 50;
	this.L = [17,29,41,53,65];
	this.H = 60;
	var clrLine = '#4c4c4c';//'#d5d5d5'
	var clrText = '#d5d5d5';if( this.BKClr == 1 )clrText = '#555555';
	var clrtooltip = '#f59300';
	this.GetLine = function(len){
		var a = 2*Math.PI/360;
		var x1 = this.x - Math.tan(a*36)*len;
		var y1 = this.y + len;
		var x2 = this.x + Math.tan(a*36)*len;
		var y2 = this.y + len;
		var x3 = x2 + Math.cos(a*72) * (Math.tan(a*36)*len*2);
		var y3 = y2 - Math.sin(a*72) * (Math.tan(a*36)*len*2);
		var x4 = this.x;
		var y4 = this.y - len / Math.cos(a*36);
		var x5 = x1 - Math.cos(a*72) * (Math.tan(a*36)*len*2);
		var y5 = y3;

		var strline = 'M'+x1+','+y1;
		strline += ' L'+x2+','+y2;
		strline += ' L'+x3+','+y3;
		strline += ' L'+x4+','+y4;
		strline += ' L'+x5+','+y5;
		strline += ' L'+x1+','+y1;

		if(len == this.L[4]){
			this.Paper.path('M'+x1+','+y1+' L'+this.x+','+this.y).attr({
				'stroke-width':1,
				'stroke':clrLine
			});
			this.Paper.path('M'+x2+','+y2+' L'+this.x+','+this.y).attr({
				'stroke-width':1,
				'stroke':clrLine
			});
			this.Paper.path('M'+x3+','+y3+' L'+this.x+','+this.y).attr({
				'stroke-width':1,
				'stroke':clrLine
			});
			this.Paper.path('M'+x4+','+y4+' L'+this.x+','+this.y).attr({
				'stroke-width':1,
				'stroke':clrLine
			});
			this.Paper.path('M'+x5+','+y5+' L'+this.x+','+this.y).attr({
				'stroke-width':1,
				'stroke':clrLine
			});
			
			this.Paper.text(x1, y1+10, "市场情绪").attr({
				'fill':clrText,
				'font-family': g_rapHanZiFontName[g_rapFontid],
				'font-size': g_rapHanZiFontSize[g_rapFontid],
				'font-weight': 'normal',
				'text-anchor': 'middle'
			});
			this.Paper.text(x2, y2+10, "外围环境").attr({
				'fill':clrText,
				'font-family': g_rapHanZiFontName[g_rapFontid],
				'font-size': g_rapHanZiFontSize[g_rapFontid],
				'font-weight': 'normal',
				'text-anchor': 'middle'
			});
			this.Paper.text(x3+3, y3, "主\n力\n资\n金").attr({
				'fill':clrText,
				'font-family': g_rapHanZiFontName[g_rapFontid],
				'font-size': g_rapHanZiFontSize[g_rapFontid],
				'font-weight': 'normal',
				'text-anchor': 'start'
			});
			this.Paper.text(x4, y4-10, "风险分析").attr({
				'fill':clrText,
				'font-family': g_rapHanZiFontName[g_rapFontid],
				'font-size': g_rapHanZiFontSize[g_rapFontid],
				'font-weight': 'normal',
				'text-anchor': 'middle'
			});
			this.Paper.text(x5-8, y5, "仓\n位\n策\n略").attr({
				'fill':clrText,
				'font-family': g_rapHanZiFontName[g_rapFontid],
				'font-size': g_rapHanZiFontSize[g_rapFontid],
				'font-weight': 'normal',
				'text-anchor': 'middle'
			});
		}
		return strline;
	}

	for(var i=0;i<this.L.length;i++){
		var strLine = this.GetLine(this.L[i]);
		this.Paper.path(strLine).attr({
			'stroke-width':1,
			'stroke':clrLine
		});
	}

	this.SetData = function(L, lr, bz, A11, B13, C13, date1, date2, date3){
		var a = 2*Math.PI/360;
		var x1 = this.x - Math.tan(a*36)*(5 + L[0]/100*this.H);
		var y1 = this.y + (5 + L[0]/100*this.H);
		var x2 = this.x + Math.tan(a*36)*(5 + L[1]/100*this.H);
		var y2 = this.y + (5 + L[1]/100*this.H);
		var x3 = this.x + Math.tan(a*36)*(5 + L[2]/100*this.H) + Math.cos(a*72) * (Math.tan(a*36)*(5 + L[2]/100*this.H)*2);
		var y3 = this.y + (5 + L[2]/100*this.H) - Math.sin(a*72) * (Math.tan(a*36)*(5 + L[2]/100*this.H)*2);
		var x4 = this.x;
		var y4 = this.y - (5 + L[3]/100*this.H) / Math.cos(a*36);
		var x5 = this.x - Math.tan(a*36)*(5 + L[4]/100*this.H) - Math.cos(a*72) * (Math.tan(a*36)*(5 + L[4]/100*this.H)*2);
		var y5 = this.y + (5 + L[4]/100*this.H) - Math.sin(a*72) * (Math.tan(a*36)*(5 + L[4]/100*this.H)*2);
		var strline = 'M'+x1+','+y1;
		strline += ' L'+x2+','+y2;
		strline += ' L'+x3+','+y3;
		strline += ' L'+x4+','+y4;
		strline += ' L'+x5+','+y5;
		strline += ' L'+x1+','+y1;

		this.Paper.path(strline).attr({
			'stroke-width':1,
			'fill':'#ffc34c',
			'opacity':'0.4',
			'stroke':'#ffc34c'
		});

		tooltip1='' + date3
		if( L[0] > 60 )
			tooltip1 += "\n市场看多情绪主导\n可保持积极心态";
		else if( L[0] >= 30 && L[0] <= 60 )
			tooltip1 += "\n市场多空情绪分歧\n需密切关注大盘变动趋势";
		else
			tooltip1 += "\n市场看空情绪较浓\n请谨慎等待投资机会";

		tooltip2='' + date2
		if( L[1] > 55 )
			tooltip2 += "\n美国道琼斯指数涨幅"+A11+"%\n德国DAX指数涨幅"+B13+"%\n日经225指数涨幅"+C13+"%\n外围股市表现强势";
		else if( L[1] >= 35 && L[1] <= 55 )
			tooltip2 += "\n美国道琼斯指数涨幅"+A11+"%\n德国DAX指数涨幅"+B13+"%\n日经225指数涨幅"+C13+"%\n外围股市表现一般";
		else
			tooltip2 += "\n美国道琼斯指数涨幅"+A11+"%\n德国DAX指数涨幅"+B13+"%\n日经225指数涨幅"+C13+"%\n外围股市表现不佳";
		
		tooltip3='' + date1
		if( L[2] > 65 )
			tooltip3 += "\n沪市主力资金净流入"+lr+"亿\n占成交量比"+bz+"%\n主力做多意愿强烈";
		else if( L[2] >= 35 && L[2] <= 65 )
			tooltip3 += "\n沪市主力资金净流入"+lr+"亿\n占成交量比"+bz+"%\n主力多空分歧较大";
		else
			tooltip3 += "\n沪市主力资金净流入"+lr+"亿\n占成交量比"+bz+"%\n主力做空意愿强烈";

		tooltip4='' + date1
		if( L[3] > 65 )
			tooltip4 += "\n大盘变盘风险较小\n预计将延续前期趋势";
		else if( L[3] >= 35 && L[3] <= 65 )
			tooltip4 += "\n大盘有一定变盘风险\n请保持警惕";
		else
			tooltip4 += "\n大盘变盘风险较大\n请关注变盘方向";

		tooltip5='' + date1
		if( L[4] > 65 )
			tooltip5 += "\n大盘趋势向好\n投资者可适当逢低增仓";
		else if( L[4] >= 35 && L[4] <= 65 )
			tooltip5 += "\n大盘趋势震荡不明\n投资者可轻仓参与";
		else
			tooltip5 += "\n大盘趋势不佳\n投资者可适当逢高减仓";


		circleclr = '#ffc34c'
		circleclr1 = '#000000'
		if ( this.BKClr == 1 )circleclr1 = '#ffffff'
		circlew = 4
		this.Elm1 = this.Paper.circle(x1, y1, circlew).attr({
			'fill':circleclr,
			'stroke':circleclr1,
			'opacity':'0.6',
			'title':tooltip1,
			'stroke-width':2
		});
		this.Elm2 = this.Paper.circle(x2, y2, circlew).attr({
			'fill':circleclr,
			'stroke':circleclr1,
			'opacity':'0.6',
			'title':tooltip2,
			'stroke-width':2
		});
		this.Elm3 = this.Paper.circle(x3, y3, circlew).attr({
			'fill':circleclr,
			'stroke':circleclr1,
			'opacity':'0.6',
			'title':tooltip3,
			'stroke-width':2
		});
		this.Elm4 = this.Paper.circle(x4, y4, circlew).attr({
			'fill':circleclr,
			'stroke':circleclr1,
			'opacity':'0.6',
			'title':tooltip4,
			'stroke-width':2
		});
		this.Elm5 = this.Paper.circle(x5, y5, circlew).attr({
			'fill':circleclr,
			'stroke':circleclr1,
			'opacity':'0.6',
			'title':tooltip5,
			'stroke-width':2
		});
	}
},

LinePic : function(){
	if(this instanceof Raphael){
		tdxPaperDraw.LinePic.Paper=this;
	}
	if(!(this instanceof tdxPaperDraw.LinePic)){
		return new tdxPaperDraw.LinePic();
	}
	if(tdxPaperDraw.LinePic.Paper){
		this.Paper=tdxPaperDraw.LinePic.Paper; //this.Paper是该画布
	}
	this.W=180;
	this.LeftW=37;
	this.BottomH=20;
	this.BKClr = 0;//0-黑色背景 1-白色背景
	this.H = 55;
	var body = document.getElementById("bodyid");
    var currenSty=getStyleEle(body)
    if(currenSty){
        var colorS=currenSty["color"]+""
        colorS=colorS.toUpperCase()

        if(colorS=='#000'||colorS=='#000000'||colorS=='RGB(0, 0, 0)'||colorS=='RGB(0,0,0)')
            this.BKClr = 1;
    }


	var clrline = '#4c4c4c';
	var clrtext = '#d5d5d5';if(this.BKClr == 1) clrtext = '#000';

	var strLine = 'M'+(this.LeftW)+','+(5);
	strLine += ' L'+(this.LeftW)+','+(this.H);
	strLine += ' L'+(this.W)+','+(this.H);
    var pNode=this.Paper.path(strLine).attr({
        'stroke-width':1,
        'stroke':clrline

    })
    if(!isIESystem){
        pNode.translate(0.5,0.5)
        pNode.node.style["shape-rendering"]="crispEdges";
    }
	this.SetData = function(LData, LDate){
		if(LData.length <= 1) return;
		var maxv = Math.max.apply(null, LData);
		var minv = Math.min.apply(null, LData);
		var xs = 1
		if( maxv >= 100 ) xs = 0
		if(Math.abs(minv)>=100) xs=0
		var sText = minv.toFixed(xs);
		sText = sText.toString();
		this.Paper.text(this.LeftW-5, this.H, sText).attr({
			'fill':clrtext,
			'font-family': g_rapShuZiFontName[g_rapFontid],
			'font-size': g_rapShuZiFontSize[g_rapFontid],
			'font-weight': 'normal',
			'text-anchor': 'end'
		});

		sText = (minv+(maxv-minv)/2).toFixed(xs);
		sText = sText.toString();
		this.Paper.text(this.LeftW-5, (this.H+5)/2, sText).attr({
			'fill':clrtext,
			'font-family': g_rapShuZiFontName[g_rapFontid],
			'font-size': g_rapShuZiFontSize[g_rapFontid],
			'font-weight': 'normal',
			'text-anchor': 'end'
		});

		sText = maxv.toFixed(xs);
		sText = sText.toString();
		this.Paper.text(this.LeftW-5, 5, sText).attr({
			'fill':clrtext,
			'font-family': g_rapShuZiFontName[g_rapFontid],
			'font-size': g_rapShuZiFontSize[g_rapFontid],
			'font-weight': 'normal',
			'text-anchor': 'end'
		});
		var bx = this.LeftW;
		var by = this.H;
		var incx = (this.W-this.LeftW)/(LData.length-1);
		var incy = (this.H-5)/(maxv-minv);
		var line = "";
		var nLen = LData.length - 1;

		if( minv < 0 && maxv > 0 ){//零线
			var y = by - (0 - minv) * incy;
			var LLine = 'M'+bx+','+y+' L'+(bx+(this.W-this.LeftW))+','+y;

			var objLine=this.Paper.path(LLine).attr({
				'stroke-width':1,
				'stroke':'#404040'
			})
            if(!isIESystem){
                objLine.translate(0.5,0.5)
                objLine.node.style["shape-rendering"]="crispEdges";

            }/*.translate(0.5,0.5)*/

            /*objLine.node.style["shape-rendering"]="crispEdges";
            console.log(objLine.node.style)*/
		}
		for(var i=0; i<LData.length;i++){
			var x = bx + i*incx;
			var y = by - (LData[i] - minv) * incy
			if( i==0 ){
				line += 'M'+x+','+y;
			}else{
				line += ' L'+x+','+y;
			}
			if( (nLen-i) % 3 == 0){
				this.Paper.text(x, this.H+10, LDate[i]).attr({
					'fill':clrtext,
					'font-family': g_rapShuZiFontName[g_rapFontid],
					'font-size': g_rapShuZiFontSize[g_rapFontid],
					'font-weight': 'normal',
					'text-anchor': 'middle'
				});
			}
		}
		var pNodes=this.Paper.path(line).attr({
			'stroke-width':1,
			'stroke':'#87a9d1'
		})


	}
}

}; //tdxPaperDraw

//Cookie
function SetCookie(name,value,extimes){
	extimes = extimes*1000; //秒
	var extime = new Date();
	extime.setTime(extime.getTime() + extimes); 
	document.cookie = name + "=" + escape(value) + ((extimes==null) ? "" : ";expires=" + extime.toGMTString());
}
function GetCookie(name){
	var arr,reg=new RegExp("(^| )"+name+"=([^;]*)(;|$)");
    if(arr = document.cookie.match(reg)){
        return unescape(arr[2]); 
    }
    else 
        return "";
}
function DelCookie(name){
	var value = GetCookie(name); 
	if(value.length>0){
		SetCookie(name,value,-1);
	}
}