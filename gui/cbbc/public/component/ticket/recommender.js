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
    this.state = {underlying: 'HSI', productCode: '', issuer: 'BI', type: 'EC', cratio: '', products: []}
    if (this.props.data3)
      this.state.underlying = this.props.data3
    this.handleChange = this.handleChange.bind(this)
    this.handleMultiSelect = this.handleMultiSelect.bind(this)
    this.handleMultiSelect2 = this.handleMultiSelect2.bind(this)
    this.handleClick = this.handleClick.bind(this)
    this.handleSelect = this.handleSelect.bind(this)
  }
  
  static getDerivedStateFromProps(props, state) {
    return state
  }
  
  componentDidMount() {
    $('[data-toggle="tooltip"]').tooltip()
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
      var url = 'https://chart.dbpower.com.hk/buysellchart/cbbc_router.php?action=spread&underlying='+that.state.underlying+'&issuer='+that.state.issuer+'&type='+that.state.type+'&sort=tt&spread=1&format=json'
      
      if (that.state.productCode.length > 0)
        url += '&codes='+that.state.productCode
      if (that.state.cratio != '' && that.state.cratio.length > 0)
        url += '&cratio='+that.state.cratio
      
      var data = await $.ajax({url: "./cross-data", type: "post", data:{url: url}})
      if (data != '')
        that.setState({products: JSON.parse(data)})
    }
    getData(this)
  }
  
  handleSelect(event) {
    event.preventDefault()
    var curCode = event.target.getAttribute('data-code')
    
    var states = this.props.getStates()
    var obj = $.extend(true, {}, states)
    var obj2 = $.extend(true, {}, states.cells)
    
    if ('modules' in obj && ('bear' in obj.modules || 'bull' in obj.modules)) {
      // get product
      var curProduct = null
      for (var i in this.state.products) {
        if (this.state.products[i].Code == curCode)
          curProduct = this.state.products[i]
      }

      var isProductExist = false
      for (var v of this.props.data2) {
        if (v.code == curProduct.Code.toString() && v.isVisable2 == true)
          isProductExist = true
      }
      if ('Type' in curProduct && (curProduct.Type.toLowerCase() == 'rc' || curProduct.Type.toLowerCase() == 'rp' || curProduct.Type.toLowerCase() == 'ec' || curProduct.Type.toLowerCase() == 'ep') && (!isProductExist)) {
        var obj3 = $.extend(true, [], states.cellsConfig)
        var i = obj3.length
        
        if (curProduct.AskIssuerSize<=0)
          curProduct.AskIssuerSize=0
        if (i>0 && obj2[i-1].action.code.value.length<5)
          i--
        obj2[i].action.code.value = curProduct.Code.toString()
        obj2[i].action.symbol.value = curProduct.UCode.toString()
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
        
        var command1 = {algo_name: algoName, cmd: 'get_warrant_detail', code: parseInt(curProduct.Code), id: states.userId, no: parseInt(i)}
        sendWebsocket(JSON.stringify(command1))
        
        // info
        obj2[i].info.issuer = curProduct.Name.substring(0, 2)
        
        //
        obj3.push({code: curProduct.Code.toString(), type: type, isVisable: false, isVisable2: true})
        this.props.setStates({'cellsConfig': obj3})
      }
    }
    
    this.props.setStates({'cells': obj2})
  }
  
  getText(lang) {
    var text = {
      en: {ucode: 'Underlying', bear: 'Bear', bull: 'Bull', call: 'Call', put: 'Put', submit: 'Submit', code: 'Code', type: 'Type', name: 'Name', lotSize: 'Lot size', bid: 'Bid', ask: 'Ask', bidIssuerSize: 'Bid Issuer Size',  askIssuerSize: 'Ask Issuer Size', spread: 'Spread', cratio: 'Cratio', select: 'Select', tickCountBid: 'Tick Count Bid', tickCountAsk: 'Tick Count Ask', lastTradeTime: 'Last Trade Time', productCode: 'Product Code', levelTime: 'Level Time', turnover: 'Turnover'},
      sc: {ucode: '标的', bear: '熊证', bull: '牛证', call: '认购', put: '认沽', submit: '提交', code: '编号', type: '种类', name: '名称', lotSize: '手数', bid: '买入价', ask: '卖出价', bidIssuerSize: '买入量',  askIssuerSize: '卖出量', spread: '格数', cratio: '对冲值', select: '选择', tickCountBid: '小单/中单/大单', tickCountAsk: '小单/中单/大单',  lastTradeTime: '最後交易时间', productCode: '产品编号', levelTime: '打和时间', turnover: '成交额'},
      tc: {ucode: '正股', bear: '熊證', bull: '牛證', call: '認購', put: '認沽', submit: '提交', code: '編號', type: '種類', name: '名稱', lotSize: '手數', bid: '買入價', ask: '賣出價', bidIssuerSize: '買入量',  askIssuerSize: '賣出量', spread: '格數', cratio: '對沖值', select: '選擇', tickCountBid: '小單/中單/大單', tickCountAsk: '小單/中單/大單', lastTradeTime: '最後交易時間', productCode: '產品編號', levelTime: '打和時間', turnover: '成交額'},
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    
    var productsHTML = []
    for (var i in this.state.products) {
      var p = this.state.products[i]
      
      // OMDD code
      var ucode = null
      if (parseInt(p.UCode) == 4001) ucode = 'HSI 恒生指数'
      else if (parseInt(p.UCode) == 4002) ucode = 'HSCE 国企指数'
      else if (parseInt(p.UCode) == 4038) ucode = 'HSTEC 恒生科技指数'
      else ucode = p.UCode+' '+getUnderlyingName2(p.UCode)
      
      productsHTML.push(
        <tr key={'product_'+i}>
          <td><a href="" data-code={p.Code} onClick={this.handleSelect}> {p.Code} </a></td>
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
          <td>{p.CountTickB.replaceAll(':', ' : ')}</td>
          <td>{p.CountTickA.replaceAll(':', ' : ')}</td>
          <td>{p.Level.replaceAll('|', ', ').replace('(', '').replace(')', '')}</td>
          <td>{p.Turnover}</td>
          <td>{moment(p.LastTradeTime,'hhmmss').format('hh:mm:ss')}</td>
        </tr>
      )
    }
    
    if (this.state.products.length == 0) {
      productsHTML.push(
        <tr key={'product_0'}>
          <td colSpan="16" className="text-center">No available products</td>
        </tr>
      )
    }
    
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
    var selOptions2 = []
    for (var [k, v] of Object.entries(wtype)) {
      selOptions2.push({value: v, label: capitalize(k)})
    }
    
    const customStyles = {
      option: (provided, state) => ({
        ...provided,
        padding: 5,
        fontSize: 12,
      }),
      menu: (provided, state) => ({
        ...provided,
      }),
      input: (provided, state) => ({
        ...provided,
        padding: 0
      }),
      control: base => ({
        ...base,
        boxShadow: 'none',
        fontSize: 14,
      }),
      singleValue: (provided, state) => {
        const opacity = 1;
        const transition = 'opacity 300ms';
        return { ...provided, opacity, transition };
      }
    }
    
    return(
<div className='row'>
<div className="col-12 col-sm-12 col-md-12 mb-1">

<div className="form-row form-recommend">
  <div className="form-group col-6 col-sm-6 col-md-1 mb-2">
    <input type="text" name="underlying" className="form-control height-adj" onChange={this.handleChange} value={this.state.underlying} />
  </div>
  
  <div className="form-group col-6 col-sm-6 col-md-1 mb-2">
    <input type="text" name="productCode" className="form-control height-adj" onChange={this.handleChange} value={this.state.productCode} placeholder={text.productCode} />
  </div>
  
  <div className="form-group col-12 col-sm-12 col-md-5 mb-2">
  <Select
    key="selector_recommender"
    options={selOptions}
    onChange={this.handleMultiSelect}
    styles={customStyles}
    placeholder=""
    isClearable={false}
    isMulti
  />
  </div>
  
  <div className="form-group col-8 col-sm-8 col-md-3 mb-2">
  <Select
    key="selector_recommender_wtype"
    options={selOptions2}
    onChange={this.handleMultiSelect2}
    styles={customStyles}
    placeholder=""
    isClearable={false}
    isMulti
  />
  </div>
  
  <div className="form-group col-4 col-sm-4 col-md-1 mb-2">
  <input type="text" name="cratio" className="form-control height-adj" onChange={this.handleChange} value={this.state.cratio} placeholder={text.cratio} />
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
    <col span="1" width="100px" />
    <col span="1" width="100px" />
    <col span="1" width="80px" />
    <col span="1" width="80px" />
    <col span="1" width="90px" />
    <col span="1" width="90px" />
    <col span="1" width="90px" />
    <col span="1" width="90px" />
    <col span="1" width="100px" />
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
    <th><span data-toggle="tooltip" data-placement="top" title="Tick Count Bid">{text.tickCountBid}</span></th>
    <th><span data-toggle="tooltip" data-placement="top" title="Tick Count Ask">{text.tickCountAsk}</span></th>
    <th><span data-toggle="tooltip" data-placement="top" title="Level Time">{text.levelTime}</span></th>
    <th>{text.turnover}</th>
    <th>{text.lastTradeTime}</th>
    </tr>
  </thead>
  <tbody>
  {productsHTML}
  </tbody>
  </table>
</div>

</div>
    )
  }
}