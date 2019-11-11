window.onerror=function(a,b,c,d,e){
  alert([a,b,c,d,e]);
}
var defaultMenus=null;
var gSort=null;
var text1='<i class="fa fa-info-circle"></i> 没有栏目哟，请从备选栏目中添加。';
var text2='<i class="fa fa-spinner fa-spin"></i> 正在加载，请稍后...';

function mouseover(obj){
  $(obj).find('i').show();
}
function mouseout(obj){
  $(obj).find('i').hide();
}

var gVm=new Vue({
  el:'#app',
  computed:{
    _selMenus:function(){
      var me=this;
      me.selMenus.map(function(item){
        item.list.map(function(item2){
          item2.my=0;
          for(var i=0;i<me.myMenus.length;i++){
            if(me.myMenus[i].mid==item2.mid){
              item2.my=1;
              break;
            }
          }
        });
      });
      return me.selMenus;
    }
  },
  methods:{
    addMenu:function(item){
      var me=this;
      if(item.my==1){
        return;
      }
      me.hasChange=true;
      if(this.limits!=-1 && this.myMenus.length>=this.limits){
        alert('对不起，您添加的自定义菜单超过了限制！');
        return;
      }
      this.myMenus.push(item);
      
      setTimeout(function(){
        document.querySelector('.my-menu-box').scrollTop=1000000;
      },1);
      me.sortit();
      me.resize();
      me.checkNoSels();
      me.setFirstShow();
    },
    delMenu:function(item,$event){
      var me=this;
      me.hasChange=true;
      for(var i=0;i<me.myMenus.length;i++){
        if(me.myMenus[i].mid==item.mid){
          me.myMenus.splice(i,1);
          break;
        }
      }
      this.sortit();
      this.resize();
      this.checkNoSels();
      this.setFirstShow();      
    },
    setFirstShow:function(){
      /*
      setTimeout(function(){
        $('.my-menu').removeClass('firstshow').each(function(){
          if($(this).index()<10){
            $(this).addClass('firstshow');
          }
        });
      },1);
      */
    },
    sortit:function(){
      var me=this;
      try{gSort.destroy();}catch(e){}
      var list = document.getElementById("sortul");
      gSort=Sortable.create(list,{
        animation: 150,
        onUpdate:function(){
          me.hasChange=true;
          me.setFirstShow();
        }
      });
    },
    saveMe:function(){
      var me=this;
      var list=[];
      var lis=document.querySelectorAll('#sortul li');
      if(lis.length==0){
        alert('没有自定义表头字段，无需保存！');
        return;
      }
      for(var i=0;i<lis.length;i++){
        list.push(lis[i].title);
      }
      var data=list.join('|');
      data='13|'+me.type+'|'+data;
      //alert('这是要回写的数据，格式为：13|<type>|<组索引>_<菜单ID>,<菜单名称>|...\r\n\r\n'+data);
      jscall({params:data},function(){
        jscall({path:'CloseIEDlg'});
      });
    },
    setDefault:function(){
      if(confirm('确认恢复到初始设置？')){
        this.emptyText=text2;
        this.myMenus=[];
        this.init();
      }
    },
    resize:function(){
      var me=this;
      var H=$(window).height();
      var H1=$('.header').outerHeight();
      var HSearch=$('.searchbox').outerHeight();
      me.H2=(H-H1-HSearch-28)+'px';
    },
    filterList:function(list){
      var me=this;
      return list.filter(function(item){
        return item.my==0 && (item.title.indexOf(me.keywords)!=-1 || me.keywords.length==0);
      })
    },
    checkNoSels:function(){
      var me=this;
      setTimeout(function(){
        me.nosels=$('.can-sel-item').length==0;
      },1);
      
    },
    clearKeywords:function(){
      this.keywords='';
    },
    init:function(){
      var me=this;
      me.type=urlparam('type');
      if(/^\d+$/.test(me.type)){    
        jscall({params:'14|'+me.type},function(error,data){
          if(error==0){
            if(/^\[\[.+\]\]$/.test(data)){
              data=data.replace('[[','').replace(',]]','');
              var list=data.split(',],[');
              var listdata=[];
              list.map(function(item,index){
                if(index==1){index=0;}
                var list2=item.split(',');
                var itemlist=[];
                for(var i=0;i<list2.length;i++){
                  if(i%2==0){
                    itemlist.push({mid:list2[i]+'',title:''});
                  }
                  else{
                    itemlist[itemlist.length-1].title=list2[i];
                  }
                }
                listdata.push({sub:index+1,list:itemlist});
              });
              defaultMenus=JSON.parse(JSON.stringify(listdata.slice(0,1)[0].list));
              me.myMenus=listdata.slice(0,1)[0].list;
              me.selMenus=listdata.slice(1);
              me.emptyText=text1;
            }
          }
          me.loadFinish=true;
          me.hasChange=false;
          setTimeout(function(){
            me.sortit();
            me.resize();
            me.checkNoSels();
            me.setFirstShow();
          },10);
        });      
      }
    }
  },
  data:{
    loadFinish:false,
    limits:-1,
    myMenus:[
      
    ],
    selMenus:[
      
    ],
    type:'',
    H2:'',
    keywords:'',
    hasChange:false,
    emptyText:text1,
    nosels:false
  },
  mounted:function(){
    var me=this;
    
    me.init();
    
    window.onresize=function(){
      me.resize();
    }
    
    $('.a-restore').click(function(){
      this.href="http://www.treeid/defaultcolorder";
      if(confirm('确认恢复所有栏目为缺省吗？')){
        setTimeout(function(){
          jscall({path:'CloseIEDlg'});
        },1);
        return true;
      }
      return false;
    });
    
  }
});