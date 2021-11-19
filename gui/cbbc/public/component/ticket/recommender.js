class Recommender extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    data2: PropTypes.array,
    data3: PropTypes.string,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {
        underlying: this.props.data3,
        productCode: '',
        issuer: Object.keys(this.props.data).join(','),
        type: 'EC,EP,RC,RP',
        cratio: '',
        products: []}
        
    this.handleChange = this.handleChange.bind(this)
    this.handleMultiSelect = this.handleMultiSelect.bind(this)
    this.handleMultiSelect2 = this.handleMultiSelect2.bind(this)
    this.handleClick = this.handleClick.bind(this)
    this.handleSelect = this.handleSelect.bind(this)
    global.sort = []
    global.lastRenderTime = null
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {
    var that = this
    
    this.initDataTable(that)
    
    $('[data-toggle="tooltip"]').tooltip()
    
    ui.slIssuer = $('.sl-issuer').select2({width: '100%'})
    ui.slIssuer.on('change', function(v) {
      var vals = ui.slIssuer.val()
      if (vals.length == 0)
        vals = Object.keys(that.props.data)
      that.setState({'issuer': vals.join(',')})
    })
    
    ui.slWtype = $('.sl-wtype').select2({width: '100%'})
    ui.slWtype.on('change', function(v) {
      var vals = ui.slWtype.val()
      if (vals.length == 0)
        vals = ['EC', 'EP', 'RC', 'RP']
      that.setState({'type': vals.join(',')})
    })
  }
  
  initDataTable(that) {
    if (('dt' in global) || global.dt) {
      global.dt.destroy()
      global.dt = null
    }
    
    var text = that.getText(that.props.lang)
    global.dt = $('.table-recommand').DataTable({
      paging: false,
      searching: false,
      info: false,
      columnDefs: [{
        targets: [0, 1, 2, 3, 4, 7, 8, 9, 10],
        orderable: false,
      },{
        type: 'num',
        targets: [5, 6]
      }],
      order: global.sort,
      fnDrawCallback: function(oSettings) {
        global.sort = [oSettings.aaSorting[0][0], oSettings.aaSorting[0][1]]
      },
      columns: [
        {data: '0'},
        {data: '1'},
        {data: '2'},
        {data: '3'},
        {data: '4'},
        {data: '5'},
        {data: '6'},
        {data: '7'},
        {data: '8'},
        {data: '9'},
        {data: '10'},
        {data: '11'},
        {data: '12'},
        {data: '13'},
        {data: '14'},
        {data: '15'},
        {data: '16'},
      ],
      /*orderFixed: {
        pre: [[16, 'desc']]
      },
      rowGroup: {
        dataSrc: 16,
        startRender: function (rows, group) {
          return '<b>'+group +' ('+rows.count()+' '+text.products+')</b>'
        }
      },*/
      createdRow: function(row, data, idx) {},
      drawCallback: function (settings) {},
      bAutoWidth: false,
    })
    
    global.dt.productList = []
    global.dt.selectProductList = []
  }
  
  updateDataTable(that) {
    var productsHTML = [],
        aaData = [],
        dom = []

    global.dt.clear()
    
    for (var i in that.state.products) {
      var p = that.state.products[i]
      
      // OMDD code
      var ucode = null
      if (parseInt(p.UCode) == 4001) ucode = 'HSI 恒生指数'
      else if (parseInt(p.UCode) == 4002) ucode = 'HSCE 国企指数'
      else if (parseInt(p.UCode) == 4038) ucode = 'HSTEC 恒生科技指数'
      else ucode = p.UCode+' '+getUnderlyingName2(p.UCode)
      
      if (p.Turnover.toLowerCase().includes('k'))
        var turnover = parseFloat(p.Turnover)*1000
      else if (p.Turnover.toLowerCase().includes('m'))
        var turnover = parseFloat(p.Turnover)*1000000
      else
        var turnover = parseFloat(p.Turnover)
      
      var lv = p.Level.replaceAll('|', ', ').replace('(', '').replace(')', '')
      
      // 
      var isProductExist = false
      if (this.props.data2) {
        for (var v of that.props.data2) {
          if (v.code == p.Code.toString() && v.isVisable2 == true)
            isProductExist = true
        }
      }
      if (global.dt.selectProductList.includes(parseInt(p.Code)))
        isProductExist = true
      
      if (isProductExist)
        var code = p.Code, code2 = p.Code
      else {
        var code = <a className="dtable-code" href="" data-code={p.Code} onClick={that.handleSelect}> {p.Code} </a>
        var code2 = '<a class="dtable-code" href="javascript:void(0);" data-code='+p.Code+'> '+p.Code+' </a>'
      }
      
      // 1. html
      productsHTML.push(
        <tr key={'product_'+i}>
          <td>{code}</td>
          <td>{ucode}</td>
          <td>{p.Type}</td>
          <td>{p.Name}</td>
          <td>{formatInputUnit(p.LotSize, false)}</td>
          <td>{p.Bid}</td>
          <td>{p.Ask}</td>
          <td>{formatInputUnit(p.BidIssuerSize, false)}</td>
          <td>{formatInputUnit(p.AskIssuerSize, false)}</td>
          <td>{p.Spread}</td>
          <td>{p.Cratio}</td>
          <td data-sort={p.CountTickB.split(':')[0]}>{p.CountTickB.replaceAll(':', ' : ')}</td>
          <td data-sort={p.CountTickA.split(':')[0]}>{p.CountTickA.replaceAll(':', ' : ')}</td>
          <td data-sort={lv.split(',')[0]}>{lv}</td>
          <td data-sort={turnover}>{p.Turnover}</td>
          <td>{moment(p.LastTradeTime,'hhmmss').format('hh:mm:ss')}</td>
        </tr>
      )
      
      // 2. aaData
      aaData.push({
        0: p.Code,
        1: ucode,
        2: p.Type,
        3: p.Name,
        4: formatInputUnit(p.LotSize, false),
        5: p.Bid.toString(),
        6: p.Ask.toString(),
        7: formatInputUnit(p.BidIssuerSize, false),
        8: formatInputUnit(p.AskIssuerSize, false),
        9: p.Spread.toString(),
        10: p.Cratio.toString(),
        11: p.CountTickB.replaceAll(':', ' : '),
        12: p.CountTickA.replaceAll(':', ' : '),
        13: lv,
        14: p.Turnover,
        15: moment(p.LastTradeTime,'hhmmss').format('hh:mm:ss')
      })
      // global.dt.rows.add(aaData)
      
      // 3. dom
      var cssHighlight = ''
      if (p.UserSelected2 == true)
        var cssHighlight = 'table-success'
      
      global.dt.rows.add($(
        '<tr class="'+cssHighlight+'">'+
        '<td>'+code2+'</td>'+
        '<td>'+ucode+'</td>'+
        '<td>'+p.Type+'</td>'+
        '<td>'+p.Name+'</td>'+
        '<td>'+formatInputUnit(p.LotSize, false)+'</td>'+
        '<td>'+p.Bid+'</td>'+
        '<td>'+p.Ask+'</td>'+
        '<td>'+formatInputUnit(p.BidIssuerSize, false)+'</td>'+
        '<td>'+formatInputUnit(p.AskIssuerSize, false)+'</td>'+
        '<td>'+p.Spread+'</td>'+
        '<td>'+p.Cratio+'</td>'+
        '<td data-sort="'+p.CountTickB.split(':')[0]+'">'+p.CountTickB.replaceAll(':', ' : ')+'</td>'+
        '<td data-sort="'+p.CountTickA.split(':')[0]+'">'+p.CountTickA.replaceAll(':', ' : ')+'</td>'+
        '<td data-sort="'+lv.split(',')[0]+'">'+lv+'</td>'+
        '<td data-sort="'+turnover+'">'+p.Turnover+'</td>'+
        '<td>'+moment(p.LastTradeTime,'hhmmss').format('HH:mm:ss')+'</td>'+
        '<td class="d-none">'+p.UserSelected+'</td>'+
        '</tr>'
      ))
    }
    
    global.dt.draw()
  }
  
  componentDidUpdate() {
    var that = this
    
    var isPopupWindow = window.location.pathname.includes('product-recommend')

    // 1. 每隔0.2s才刷新
    var isRefresh1 = true
    if (global.lastRenderTime != null)
      var isRefresh1 = (moment().diff(global.lastRenderTime, 'milliseconds') > 200)
    
    // 2. 產品不同, 強制刷新
    var productList = []
    for (var v of this.state.products)
      productList.push(v.Code)
    function arraysEqual(a, b) {
      a = Array.isArray(a) ? a : []
      b = Array.isArray(b) ? b : []
      return a.length === b.length && a.every((el, ix) => el === b[ix])
    }
    var isEqual1 = arraysEqual(productList, global.dt.productList)
    
    // 3. 已剔選產品
    var selectProductList = []
    if (this.props.data2) {
      for (var v of this.props.data2)
        selectProductList.push(parseInt(v.code))
    }
    
    // 4.0 同步數據
    var isRefresh2 = true, isRefresh3 = true
    if (isPopupWindow) {
      var states = this.props.getStates(),
          obj = $.extend(true, [], states.cells),
          obj2 = $.extend(true, {}, states.cellsConfig),
          count = 0
          
      for (var v of obj) {
        // 4.1 已返回underlying code, 才能刷新
        if (v.action.code.value.toString().length > 0 && v.action.symbol.value.toString().length == 0)
          isRefresh2 = false
        // 4.2 已同步cells & cellsConfig, 才能刷新
        if (v.action.code.value.toString().length > 0)
          count++
      }
      
      if (Object.keys(obj2).length != count)
        isRefresh3 = false
    }
    
    if (!isPopupWindow || (isPopupWindow && (!isEqual1 || (isRefresh1 && isRefresh2 && isRefresh3)))) {
      global.lastRenderTime = moment()
      
      this.updateDataTable(this)
      
      ui.dtableCode = $('.dtable-code')
      ui.dtableCode.click(function() {
        var code = $(this).data('code')
        that.handleSelect(code, that)
      })
      
      global.dt.productList = productList
      global.dt.selectProductList = selectProductList
    }
  }
  
  handleChange(event) {
    var {name, value} = event.target
    value = value.replace(/\s/g, '');
    
    if (name == 'underlying')
      this.setState({'underlying': value})
    else if (name == 'productCode')
      this.setState({'productCode': value})
    else if (name == 'issuer')
      this.setState({'issuer': value})
    else if (name == 'type')
      this.setState({'type': value})
    else if (name == 'cratio')
      this.setState({'cratio': value})
  }
  
  handleMultiSelect(event) {
    var issuer = []
    for (var v of event)
      issuer.push(v.value)
    
    if (issuer.length == 0)
      issuer = Object.keys(this.props.data)
    
    this.setState({'issuer': issuer.join(',')})
  }
  
  handleMultiSelect2(event) {
    var wtypes = []
    for (var v of event)
      wtypes.push(v.value)
    this.setState({'type': wtypes.join('')})
  }
  
  handleClick(event) {
    async function getData(that) {
      var text = that.getText(that.props.lang)
      var url = 'https://chart.dbpower.com.hk/buysellchart/cbbc_router.php?action=spread&underlying='+that.state.underlying+'&issuer='+that.state.issuer+'&type='+that.state.type+'&sort=tt&spread=1&format=json'
      
      if (that.state.productCode.length > 0)
        url += '&codes='+that.state.productCode
      if (that.state.cratio != '' && that.state.cratio.length > 0)
        url += '&cratio='+that.state.cratio
      
      var data = await $.ajax({url: "./cross-data", type: "post", data:{url: url}})
      if (data != '') {
        var data2 = JSON.parse(data)
        for (var i in data2) {
          data2[i].UserSelected = text.autoSearch, data2[i].UserSelected2 = false
          if (that.state.productCode.split(',').includes(data2[i].Code.toString()))
            data2[i].UserSelected = text.customDefine, data2[i].UserSelected2 = true
        }
        that.setState({products: data2})
      }
    }
    getData(this)
  }
  
  handleSelect(code, that) {
    ui.dtableCode = $('.dtable-code')
    ui.dtableCode.css({'pointer-events': 'none', 'opacity': '.25'})
    
    var curCode = code
    var states = that.props.getStates()
    var obj = $.extend(true, {}, states)
    var obj2 = $.extend(true, [], states.cells)
    var obj3 = $.extend(true, [], states.cellsConfig)
    var command1 = ''
    
    if ('modules' in obj && ('bear' in obj.modules || 'bull' in obj.modules)) {
      // get product
      var curProduct = null
      for (var i in that.state.products) {
        if (that.state.products[i].Code == curCode)
          curProduct = that.state.products[i]
      }

      var isProductExist = false
      for (var v of that.props.data2) {
        if (v.code == curProduct.Code.toString() && v.isVisable2 == true)
          isProductExist = true
      }
      if ('Type' in curProduct && (curProduct.Type.toLowerCase() == 'rc' || curProduct.Type.toLowerCase() == 'rp' || curProduct.Type.toLowerCase() == 'ec' || curProduct.Type.toLowerCase() == 'ep') && (!isProductExist)) {
        var i = obj3.length
        
        if (curProduct.AskIssuerSize<=0)
          curProduct.AskIssuerSize=0
        if (i>0 && obj2[i-1].action.code.value.length<5)
          i--
        
        obj2[i].action.code.value = curProduct.Code.toString()
        obj2[i].action.symbol.value = '' // curProduct.UCode.toString()
        var AskIssuerSize = parseInt(curProduct.AskIssuerSize*0.8/1000000)*1000000
        obj2[i].action.issuerSize.value = formatInputUnit(AskIssuerSize, false)
        obj2[i].action.quantity.value = formatInputUnit(curProduct.LotSize, false)
        obj2[i].action.spread.value = curProduct.Spread.toString()
        obj2[i].action.delta.value = '0'
        
        if (curProduct.Type.toLowerCase() == 'rc')
          var algoName = obj.modules.bull, type = 'bull'
        else if (curProduct.Type.toLowerCase() == 'rp')
          var algoName = obj.modules.bear, type = 'bear'
        else if (curProduct.Type.toLowerCase() == 'ec')
          var algoName = obj.modules.bull, type = 'bull'
        else if (curProduct.Type.toLowerCase() == 'ep')
          var algoName = obj.modules.bear, type = 'bear'
        
        command1 = {algo_name: algoName, cmd: 'get_warrant_detail', code: parseInt(curProduct.Code), id: states.userId, no: parseInt(i)}
        sendWebsocket(JSON.stringify(command1))
        
        // info
        obj2[i].info.issuer = curProduct.Name.substring(0, 2)
        
        //
        obj3.push({code: curProduct.Code.toString(), type: type, isVisable: false, isVisable2: true})
        that.props.setStates({'cellsConfig': obj3})
        
        //
        global.dt.selectProductList.push(parseInt(curProduct.Code))
      }
    }
    
    that.props.setStates({'cells': obj2})
    
    // send to parent window
    if (window.opener) {
      var data = {action: 'send_msg', cellsConfig: obj3, cells: obj2, command1: command1}
      window.opener.postMessage(data)
    }
  }
  
  getText(lang) {
    var text = {
      en: {ucode: 'Underlying', bear: 'Bear', bull: 'Bull', call: 'Call', put: 'Put', submit: 'Submit', code: 'Code', type: 'Type', name: 'Name', lotSize: 'Lot size', bid: 'Bid', ask: 'Ask', bidIssuerSize: 'Bid Issuer Size',  askIssuerSize: 'Ask Issuer Size', spread: 'Spread', cratio: 'Cratio', select: 'Select', tickCountBid: 'Tick Count Bid', tickCountAsk: 'Tick Count Ask', lastTradeTime: 'Last Trade Time', productCode: 'Product Code', levelTime: 'Level Time', turnover: 'Turnover', products: 'Products', customDefine: 'Custom Define', autoSearch: 'Auto Search'},
      sc: {ucode: '标的', bear: '熊证', bull: '牛证', call: '认购', put: '认沽', submit: '提交', code: '编号', type: '种类', name: '名称', lotSize: '手数', bid: '买入价', ask: '卖出价', bidIssuerSize: '买入量',  askIssuerSize: '卖出量', spread: '格数', cratio: '对冲值', select: '选择', tickCountBid: '小单/中单/大单', tickCountAsk: '小单/中单/大单',  lastTradeTime: '最後交易时间', productCode: '产品编号', levelTime: '打和时间', turnover: '成交额', products: '个产品', customDefine: '自定义', autoSearch: '自动搜索'},
      tc: {ucode: '正股', bear: '熊證', bull: '牛證', call: '認購', put: '認沽', submit: '提交', code: '編號', type: '種類', name: '名稱', lotSize: '手數', bid: '買入價', ask: '賣出價', bidIssuerSize: '買入量',  askIssuerSize: '賣出量', spread: '格數', cratio: '對沖值', select: '選擇', tickCountBid: '小單/中單/大單', tickCountAsk: '小單/中單/大單', lastTradeTime: '最後交易時間', productCode: '產品編號', levelTime: '打和時間', turnover: '成交額', products: '個產品', customDefine: '自定義', autoSearch: '自動搜索'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    
    var selOptions = []
    for (const [k, v] of Object.entries(this.props.data)) {
      selOptions.push({value: k, label: k+' ('+v[this.props.lang]+') '})
    }
    
    var wtype = {
      call: 'EC',
      put: 'EP',
      bull: 'RC',
      bear: 'RP'
    }
    var selOptions2 = [], selOptions3 = []
    for (var [k, v] of Object.entries(wtype)) {
      selOptions2.push({value: v, label: capitalize(k)})
      selOptions3.push(
        <option key={'sl-issuer-'+v} value={v}> {capitalize(k)} </option>
      )
    }
    
    var selOptions4 = []
    for (var [k, v] of Object.entries(this.props.data)) {
      selOptions4.push(
        <option key={'sl-issuer-'+k} value={k}>{k} {v[this.props.lang]}</option>
      )
    }
    
    return(
<div className='div-recommand'>
<div className='row'>
<div className={classNames("col-12 col-sm-12 col-md-12 mb-1")}>

<div className="form-row form-recommend">
  <div className="form-group col-6 col-sm-6 col-md-1 mb-2">
    <input type="text" name="underlying" className="form-control height-adj" onChange={this.handleChange} value={this.state.underlying} autoComplete="off"/>
  </div>
  
  <div className="form-group col-6 col-sm-6 col-md-1 mb-2">
    <input type="text" name="productCode" className="form-control height-adj" onChange={this.handleChange} value={this.state.productCode} placeholder={text.productCode} autoComplete="off"/>
  </div>
  
  <div className="form-group col-12 col-sm-12 col-md-5 mb-2">
  <Select
    className="d-none"
    key="selector_recommender"
    options={selOptions}
    onChange={this.handleMultiSelect}
    placeholder=""
    isClearable={false}
    isMulti
  />
  <select className="form-control sl-issuer" name="sl-issuer" multiple="multiple" onChange={this.handleMultiSelect3} >
    {selOptions4}
  </select>
  </div>
  
  <div className="form-group col-8 col-sm-8 col-md-3 mb-2">
  <Select
    className="d-none"
    key="selector_recommender_wtype"
    options={selOptions2}
    onChange={this.handleMultiSelect2}
    placeholder=""
    isClearable={false}
    isMulti
  />
  <select className="form-control sl-wtype" name="sl-wtype" multiple="multiple" onChange={this.handleMultiSelect3} >
    {selOptions3}
  </select>
  </div>
  
  <div className="form-group col-4 col-sm-4 col-md-1 mb-2">
  <input type="text" name="cratio" className="form-control height-adj" onChange={this.handleChange} value={this.state.cratio} placeholder={text.cratio} autoComplete="off" />
  </div>
  
  <div className="form-group col-12 col-sm-12 col-md-1 mb-2">
    <button name="send" type="button" className="btn btn-sm btn-secondary mb-0 height-adj" onClick={this.handleClick}>{text.submit}</button>
  </div>
</div>
</div>

<div className="col-12 col-sm-12 col-md-12 mb-2">
  <table className="table table-sm table-striped table-light table-recommand">
  <colgroup>
    <col span="1" width="50px" />
    <col span="1" width="150px" />
    <col span="1" width="80px" />
    <col span="1" width="150px" />
    <col span="1" width="80px" />
    <col span="1" width="80px" />
    <col span="1" width="80px" />
    <col span="1" width="110px" />
    <col span="1" width="110px" />
    <col span="1" width="80px" />
    <col span="1" width="80px" />
    <col span="1" width="110px" />
    <col span="1" width="110px" />
    <col span="1" width="90px" />
    <col span="1" width="90px" />
    <col span="1" width="110px" />
  </colgroup>
  <thead>
    <tr>
    <th>{text.code}</th>
    <th>{text.ucode}</th>
    <th>{text.type}</th>
    <th>{text.name}</th>
    <th>{text.lotSize}</th>
    <th>{text.bid}</th>
    <th>{text.ask}</th>
    <th>{text.bidIssuerSize}</th>
    <th>{text.askIssuerSize}</th>
    <th>{text.spread}</th>
    <th>{text.cratio}</th>
    <th><span data-toggle="tooltip" data-placement="top" title="<1 tick | 1~4 ticks | >4 ticks">{text.tickCountBid}</span></th>
    <th><span data-toggle="tooltip" data-placement="top" title="<1 tick | 1~4 ticks | >4 ticks">{text.tickCountAsk}</span></th>
    <th><span data-toggle="tooltip" data-placement="top" title="<5s | 5~10s | 10~30s | 30~60s | >60s">{text.levelTime}</span></th>
    <th>{text.turnover}</th>
    <th>{text.lastTradeTime}</th>
    <th className="d-none">UserSelected</th>
    </tr>
  </thead>
  <tbody>
  
  </tbody>
  </table>
</div>

</div>
</div>
    )
  }
}