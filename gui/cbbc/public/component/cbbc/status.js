class Status extends React.Component {
  static propTypes = {
    data: PropTypes.object,
    lang: PropTypes.string,
    setStates: PropTypes.func,
    getStates: PropTypes.func
  }
  
  constructor(props) {
    super(props)
    this.state = {isAlive: true, totalBytes: null, noPackage: null, lastAliveTime: null}
    this.handleClick = this.handleClick.bind(this)
    this.handleClick2 = this.handleClick2.bind(this)
    
    setInterval(() => {
      var diff = (Date.now() - this.state.lastAliveTime)/1000
      if (diff>30) {
        this.state.isAlive = false
        this.forceUpdate()
      }
    }, 1000*10)
    
    setInterval(() => {
      if (Cookies.get("cbbc-uid") === undefined) {
        alert('Authentication Token Failure.');
        global.func.logout();
      }
    }, 1000*2)
    
  }
  
  static getDerivedStateFromProps(props, state) {
    state.totalBytes = props.data.totalBytes
    state.noPackage = props.data.noPackage
    state.lastAliveTime = props.data.lastAliveTime
    
    var states = props.getStates(),
        cellsConfig = $.extend(true, [], props.data3),
        cells = $.extend(true, [], states.cells),
        modules = $.extend(true, [], states.modules)
        
    var modules2 = []
    for (var [k, v] of Object.entries(modules))
      modules2.push(k+','+v)
    
    if (props.window1) {
      setTimeout(function() {
        var data4 = {action: 'send_msg', cellsConfig: cellsConfig, cells: cells, modules: modules2, userId: states.userId}
        props.window1.postMessage(data4, '*')
      }, 500)
    }
    
    return state
  }
  
  componentDidMount() {
    var that = this
    window.addEventListener('message', function(e) {
      if (('action' in event.data) && event.data.action == 'send_msg') {
        console.log(event.data)
        that.props.setStates({cellsConfig: event.data.cellsConfig})
        that.props.setStates({cells: event.data.cells})
        
        sendWebsocket(JSON.stringify(event.data.command1))
      }
    })
  }
  
  handleClick() {
    var name = event.target.name,
        data2 = $.extend(true, {}, this.props.data2)
    
    if (name == 'recommender') {
      if (data2.recommender == false) data2.recommender = true
      else if (data2.recommender == true) data2.recommender = false
    }
    
    else if (name == 'realTimeData') {
      if (data2.realTimeData == false) data2.realTimeData = true
      else if (data2.realTimeData == true) data2.realTimeData = false
    }
    
    else if (name == 'recommenderV2') {
      if (data2.recommenderV2 == false) {
        data2.recommenderV2 = true
        var windowRecommend = window.open('./product-recommend')
      }
      else if (data2.recommenderV2 == true) {
        this.props.window1.close()
        data2.recommenderV2 = false
        var windowRecommend = null
      }
      this.props.setStates({windowRecommend: windowRecommend})
    }
    
    else if (name == 'strategy1') {
      if (data2.strategy1 == false) data2.strategy1 = true
      else if (data2.strategy1 == true) data2.strategy1 = false
    }
    
    this.props.setStates({visible: data2})
  }
  
  async handleClick2() {
    var name = event.target.name,
        states = this.props.getStates(),
        obj = $.extend(true, [], states.cellsConfig),
        obj2 = $.extend(true, {}, states.cells)
    
    if (name == 'updateRealTimeData' && obj.length > 0) {
      var params = []
      for (var v of obj)
        if (v.code && v.code != '' && v.code.length >=5)
          params.push(v.code)
        else
          params.push(0)
      
      var url = 'https://chart.dbpower.com.hk/buysellchart/cbbc_router.php?action=price&code='+params.join(',')+'&format=json'
      var data = await $.ajax({url: "./cross-data", type: "post", data:{url: url}})
      data = JSON.parse(data)
      
      var no = 0
      for (var v of data) {
        while (params[no] == 0)
          no++
        
        obj2[no].wPrice.bid = v.BestBid
        obj2[no].wPrice.ask = v.BestAsk
        obj2[no].wPrice.ipriceAsk = v.IBestAsk
        obj2[no].wPrice.ipriceBid = v.IBestBid
        
        obj2[no].wPrice.ipriceAskIsWrong = false
        obj2[no].wPrice.ipriceBidIsWrong = false
        
        no++
      }
      this.props.setStates({cells: obj2})
    }
  }
  
  getText(lang) {
    var text = {
      en: {totalBytes: 'Data Transferred', noPackage: 'Data packet', lastAliveTime: 'Last Connect Time', connect: 'Connected', close: 'Closed', recommender: ' Recommender', popUpRecommender: 'Recommender (PopUp)', realTimeData: 'Real Time Data', updateRealTimeData: 'Refresh Data', strategy: 'Strategy'},
      sc: {totalBytes: '使用量', noPackage: '数据包', lastAliveTime: '最後连接时间', connect: '连接中', close: '已掉线', recommender: '产品推荐', popUpRecommender: '產品推薦 (新視窗)', realTimeData: '即市数据', updateRealTimeData: '刷新即市数据', strategy: '策略'},
      tc: {totalBytes: '使用量', noPackage: '數據包', lastAliveTime: '最後連接時間', connect: '连接中', close: '已掉线', recommender: '產品推薦', popUpRecommender: '产品推荐 (新视窗)', realTimeData: '即市數據', updateRealTimeData: '刷新即市數據', strategy: '策略'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var {isAlive, totalBytes, noPackage, lastAliveTime} = this.state
    var connectStatus = (isAlive) ? text.connect : text.close
    var style1 = (isAlive) ? 'btn-success': 'btn-danger'
    var style2 = (this.props.data2.recommender) ? 'btn-dark' : 'btn-secondary'
    var style3 = (this.props.data2.realTimeData) ? 'btn-dark' : 'btn-secondary'
    var style4 = (this.props.data2.recommenderV2) ? 'btn-dark' : 'btn-secondary'
    var style5 = (this.props.data2.strategy1) ? 'btn-dark' : 'btn-secondary'
    
    return(
      <div className='row'>
        <div className="col-12 col-sm-12 col-md-12 mb-3">
          <div className="status">
            <button className={classNames("btn status-icon mr-2 mt-2 mt-md-0", style1)} data-toggle="collapse" data-target="#status_detail" aria-expanded="false" aria-controls="status_detail">
              {connectStatus}
            </button>
            {this.props.data2.btnRecommender && <button name="recommender" className={classNames("btn status-icon mr-2 mt-2 mt-md-0", style2)} onClick={this.handleClick}> {text.recommender} </button>}
            {this.props.data2.btnRecommender && <button name="recommenderV2" className={classNames("btn status-icon mr-2 mt-2 mt-md-0", style4)} onClick={this.handleClick}> {text.popUpRecommender} </button>}
            <span className="mr-2"> | </span>
            <button name="realTimeData" className={classNames("btn status-icon mr-2 mt-2 mt-md-0", style3)} onClick={this.handleClick}> {text.realTimeData} </button>
            <button name="updateRealTimeData" className="btn status-icon btn-secondary mr-2 mt-2 mt-md-0" onClick={this.handleClick2}> {text.updateRealTimeData} </button>
            
            <span className="mr-2"> | </span>
            <button name="strategy1" className={classNames("btn status-icon mr-2 mt-2 mt-md-0", style5)} onClick={this.handleClick}> {text.strategy+'-001'} </button>
            
            <div className="collapse" id="status_detail">
              <div className="card card-body">
                {text.totalBytes}： {numberWithCommas(totalBytes)} Bytes <br />
                {text.noPackage}： {noPackage} <br />
                {text.lastAliveTime}： {moment(lastAliveTime).format("YYYY-MM-DD HH:mm:ss")} <br />
              </div>
            </div>

          </div>
        </div>
      </div>
    )
  }
}