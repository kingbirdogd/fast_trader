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
    
    setInterval(() => {
      var diff = (Date.now() - this.state.lastAliveTime)/1000
      if (diff>30) {
        this.state.isAlive = false
        this.forceUpdate()
      }
    }, 1000*10)
    
    setInterval(() => {
      if (Cookies.get("s1csalgo-uid") === undefined) {
        alert('Authentication Token Failure.');
        global.func.logout();
      }
    }, 1000*2)
    
  }
  
  static getDerivedStateFromProps(props, state) {
    state.totalBytes = props.data.totalBytes
    state.noPackage = props.data.noPackage
    state.lastAliveTime = props.data.lastAliveTime
    return state
  }
  
  componentDidMount() {}
  
  getText(lang) {
    var text = {
      en: {totalBytes: 'Data Transferred', noPackage: 'Data packet', lastAliveTime: 'Last Connect Time', connect: 'Connected', close: 'Closed'},
      sc: {totalBytes: '使用量', noPackage: '数据包', lastAliveTime: '最後连接时间', connect: '连接中', close: '已掉线'},
      tc: {totalBytes: '使用量', noPackage: '數據包', lastAliveTime: '最後連接時間', connect: '连接中', close: '已掉线'}
    }
    return text[lang]
  }
  
  render() {
    var text = this.getText(this.props.lang)
    var {isAlive, totalBytes, noPackage, lastAliveTime} = this.state
    var connectStatus = (isAlive) ? text.connect : text.close
    var iconStyle = (isAlive) ? 'btn-success': 'btn-danger'
    return(
      <div className='row'>
        <div className="col-12 col-sm-6 col-md-3 mb-3">
          <div className="status">
            <button className={classNames("btn status-icon", iconStyle)} data-toggle="collapse" data-target="#status_detail" aria-expanded="false" aria-controls="status_detail">
              {connectStatus}
            </button>
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